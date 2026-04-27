#pragma once
#include <QImage>
#include <QString>
#include <array>
#include <onnxruntime_cxx_api.h>
#include <vector>

struct WitherResult
{
    int cls = -1;                // 0/1/2
    std::array<float, 3> prob{}; // softmax 后概率
    std::array<float, 3> logits{};
};

class WitherOnnxInfer
{
public:
    bool init(const QString &onnxPath);
    bool infer(const QImage &img224rgb, const std::vector<float> &nir128, WitherResult &out);

private:
    Ort::Env m_env{ORT_LOGGING_LEVEL_WARNING, "wither"};
    Ort::SessionOptions m_opt;
    std::unique_ptr<Ort::Session> m_sess;

    std::string m_inImageName;
    std::string m_inNirName;
    std::string m_outName;
};
