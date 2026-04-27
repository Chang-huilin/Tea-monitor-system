#include "WitherOnnxInfer.h"
#include <algorithm>
#include <cmath>

static void softmax3(const std::array<float,3>& x, std::array<float,3>& p) {
    float m = std::max({x[0], x[1], x[2]});
    float e0 = std::exp(x[0]-m);
    float e1 = std::exp(x[1]-m);
    float e2 = std::exp(x[2]-m);
    float s = e0+e1+e2;
    p[0]=e0/s; p[1]=e1/s; p[2]=e2/s;
}

static int argmax3(const std::array<float,3>& p) {
    int k=0;
    if (p[1]>p[k]) k=1;
    if (p[2]>p[k]) k=2;
    return k;
}

// QImage(224x224, RGB888) -> float NCHW (1,3,224,224)
// 注意：归一化必须与训练一致；这里先做 0..1，后续你再对齐训练的 mean/std
static bool qimageToNCHWFloat(const QImage& img224rgb, std::vector<float>& out) {
    if (img224rgb.isNull()) return false;
    QImage img = img224rgb;
    if (img.format() != QImage::Format_RGB888)
        img = img.convertToFormat(QImage::Format_RGB888);
    if (img.width()!=224 || img.height()!=224) return false;

    out.assign(1 * 3 * 224 * 224, 0.0f);

    const int W = 224, H = 224;
    for (int y=0; y<H; ++y) {
        const uchar* row = img.constScanLine(y);
        for (int x=0; x<W; ++x) {
            const int idx = x * 3;
            float r = row[idx + 0] / 255.0f;
            float g = row[idx + 1] / 255.0f;
            float b = row[idx + 2] / 255.0f;

            // NCHW
            out[0*H*W + y*W + x] = r;
            out[1*H*W + y*W + x] = g;
            out[2*H*W + y*W + x] = b;
        }
    }
    return true;
}

bool WitherOnnxInfer::init(const QString& onnxPath) {
    m_opt.SetIntraOpNumThreads(1);
    m_opt.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    std::wstring w = onnxPath.toStdWString();
    m_sess = std::make_unique<Ort::Session>(m_env, w.c_str(), m_opt);

    Ort::AllocatorWithDefaultOptions alloc;

    // 假设模型输入顺序是 [image, nir]，输出 [logits]；如不一致，用名字匹配更稳
    auto in0 = m_sess->GetInputNameAllocated(0, alloc);
    auto in1 = m_sess->GetInputNameAllocated(1, alloc);
    auto out0 = m_sess->GetOutputNameAllocated(0, alloc);

    m_inImageName = in0.get();
    m_inNirName   = in1.get();
    m_outName     = out0.get();

    // 如果你不确定顺序，可打印 GetInputNameAllocated 看名字是不是 "image"/"nir"
    return true;
}

bool WitherOnnxInfer::infer(const QImage& img224rgb, const std::vector<float>& nir128, WitherResult& out) {
    if (!m_sess) return false;
    if ((int)nir128.size() != 128) return false;

    std::vector<float> imgTensor;
    if (!qimageToNCHWFloat(img224rgb, imgTensor)) return false;

    // image: [1,3,224,224]
    std::array<int64_t,4> imgShape{1,3,224,224};
    // nir: [1,128]
    std::array<int64_t,2> nirShape{1,128};

    Ort::MemoryInfo mem = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value imgV = Ort::Value::CreateTensor<float>(
        mem, imgTensor.data(), imgTensor.size(), imgShape.data(), imgShape.size());

    // nir128 需要连续内存；传入 vector.data()
    Ort::Value nirV = Ort::Value::CreateTensor<float>(
        mem, const_cast<float*>(nir128.data()), nir128.size(), nirShape.data(), nirShape.size());

    const char* inNames[]  = { m_inImageName.c_str(), m_inNirName.c_str() };
    Ort::Value  inVals[]   = { std::move(imgV), std::move(nirV) };
    const char* outNames[] = { m_outName.c_str() };

    auto outs = m_sess->Run(Ort::RunOptions{nullptr}, inNames, inVals, 2, outNames, 1);
    float* logits = outs[0].GetTensorMutableData<float>();

    out.logits = { logits[0], logits[1], logits[2] };
    softmax3(out.logits, out.prob);
    out.cls = argmax3(out.prob);
    return true;
}
