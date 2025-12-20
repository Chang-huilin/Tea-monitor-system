#include "camera_service.h"     // 必须第一行

#include <PvSystem.h>
#include <PvInterface.h>
#include <PvDeviceInfo.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvImage.h>
#include <PvPixelType.h>
#include <sstream>
#include <PvBufferConverter.h>

#include <QString>
#include <QDebug>
#include <QImage>


CameraService::CameraService() = default;

CameraService::~CameraService()
{
    if (m_device)
    {
        PvDevice::Free(static_cast<PvDevice*>(m_device));
        m_device = nullptr;
    }
}

bool CameraService::open()
{
    if (m_device)
        return true;

    if (!m_selectedDeviceInfo)
        return false;

    const PvDeviceInfo* info =
        static_cast<const PvDeviceInfo*>(m_selectedDeviceInfo);

    PvResult result;
    PvDevice* device = PvDevice::CreateAndConnect(info, &result);

    if (!device || !result.IsOK())
    {
        qDebug() << "CreateAndConnect failed:"
                 << result.GetDescription().GetAscii();
        return false;
    }

    qDebug() << "Device IsConnected =" << device->IsConnected();


    // ===============================
    // GenICam 参数
    // ===============================
    PvGenParameterArray* params = device->GetParameters();
    if (params)
    {
        // 打印设备信息
        PvString vendor, model;
        if (params->GetStringValue("DeviceVendorName", vendor).IsOK() &&
            params->GetStringValue("DeviceModelName", model).IsOK())
        {
            qDebug() << "Camera connected:"
                     << vendor.GetAscii()
                     << model.GetAscii();
            m_cameraModel =
                QString("%1 %2")
                    .arg(vendor.GetAscii())
                    .arg(model.GetAscii());
        }

        // 打开自动白平衡
        PvResult wbResult =
            params->SetEnumValue("BalanceWhiteAuto", "Continuous");

        if (wbResult.IsOK())
        {
            qDebug() << "Auto white balance enabled";
        }
        else
        {
            qDebug() << "Auto white balance not supported";
        }
    }

    m_device = device;
    return true;
}


bool CameraService::grabOnce(QImage& outImage)
{
    if (!m_device || !m_selectedDeviceInfo)
        return false;

    auto* device = static_cast<PvDevice*>(m_device);
    const auto* info =
        static_cast<const PvDeviceInfo*>(m_selectedDeviceInfo);

    // 1. 打开 Stream
    PvResult result;
    PvStream* stream = PvStream::CreateAndOpen(info, &result);
    if (!stream || !result.IsOK())
    {
        qDebug() << "[grab] CreateAndOpen stream failed";
        return false;
    }

    // 2. 创建 Pipeline
    PvPipeline pipeline(stream);
    pipeline.SetBufferCount(4);

    // 3. 启动 Pipeline（必须检查）
    PvResult startResult = pipeline.Start();
    if (!startResult.IsOK())
    {
        qDebug() << "[grab] Pipeline start failed";
        PvStream::Free(stream);
        return false;
    }
    // ===== GEV 专用：配置数据流目的地 =====
    PvDeviceGEV* gevDevice = dynamic_cast<PvDeviceGEV*>(device);
    PvStreamGEV* gevStream = dynamic_cast<PvStreamGEV*>(stream);

    if (gevDevice && gevStream)
    {
        // 1. 自动协商包大小（非常关键）
        PvResult r1 = gevDevice->NegotiatePacketSize();
        qDebug() << "[grab] NegotiatePacketSize:" << r1.IsOK();

        // 2. 设置 Stream Destination（告诉相机图像发到哪）
        PvResult r2 = gevDevice->SetStreamDestination(
            gevStream->GetLocalIPAddress(),
            gevStream->GetLocalPort()
            );
        qDebug() << "[grab] SetStreamDestination:" << r2.IsOK();

        if (!r1.IsOK() || !r2.IsOK())
        {
            qDebug() << "[grab] GEV stream configuration failed";
            pipeline.Stop();
            PvStream::Free(stream);
            return false;
        }
    }
    else
    {
        qDebug() << "[grab] Not a GEV device/stream";
    }

    PvGenParameterArray* params = device->GetParameters();
    if (!params)
    {
        qDebug() << "[grab] GetParameters failed";
        return false;
    }

    // 1. 设为连续采集
    params->SetEnumValue("AcquisitionMode", "Continuous");

    // 2. 关闭触发
    params->SetEnumValue("TriggerMode", "Off");

    // 3. 防御性设置（即使没用也不报错）
    params->SetEnumValue("TriggerSource", "Software");

    qDebug() << "[grab] AcquisitionMode=Continuous, TriggerMode=Off";

    // 4. 启动采集
    PvResult streamEnable = device->StreamEnable();
    if (!streamEnable.IsOK())
    {
        qDebug() << "[grab] StreamEnable failed";
        pipeline.Stop();
        PvStream::Free(stream);
        return false;
    }

    PvResult acqStart =
        device->GetParameters()->ExecuteCommand("AcquisitionStart");
    if (!acqStart.IsOK())
    {
        qDebug() << "[grab] AcquisitionStart failed";
        device->StreamDisable();
        pipeline.Stop();
        PvStream::Free(stream);
        return false;
    }

    // 5. 取一帧
    PvBuffer* buffer = nullptr;
    PvResult opResult;
    PvResult grabResult =
        pipeline.RetrieveNextBuffer(&buffer, 1000, &opResult);

    // 6. 停止采集
    device->GetParameters()->ExecuteCommand("AcquisitionStop");
    device->StreamDisable();

    pipeline.Stop();
    PvStream::Free(stream);

    if (!grabResult.IsOK() || !opResult.IsOK() || !buffer)
    {
        qDebug() << "[grab] RetrieveNextBuffer failed";
        return false;
    }

    // =========================================================
    // 7. PixelType → QImage
    // =========================================================
    PvImage* image = buffer->GetImage();
    if (!image)
        return false;

    PvPixelType pixelType = image->GetPixelType();

    uint32_t width   = image->GetWidth();
    uint32_t height  = image->GetHeight();
    uint32_t padding = image->GetPaddingX();

    const uint8_t* data = image->GetDataPointer();

    // ---------- Mono8 ----------
    if (pixelType == PvPixelMono8)
    {
        uint32_t stride = width + padding;

        QImage qimg(
            data,
            width,
            height,
            stride,
            QImage::Format_Grayscale8
            );

        outImage = qimg.copy();
    }
    // ---------- Bayer RAW → RGB ----------
    else if (
        pixelType == PvPixelBayerRG8 ||
        pixelType == PvPixelBayerBG8 ||
        pixelType == PvPixelBayerGR8 ||
        pixelType == PvPixelBayerGB8
        )
    {
        static PvBufferConverter converter;

        converter.SetBayerFilter(PvBayerFilterSimple);


        PvBuffer rgbBuffer;

        // ⭐ 关键：通过 PvImage 分配 RGB 图像
        PvImage* rgbImage = rgbBuffer.GetImage();
        if (!rgbImage)
        {
            qDebug() << "[grab] GetImage from rgbBuffer failed";
            return false;
        }

        PvResult allocResult = rgbImage->Alloc(
            width,
            height,
            PvPixelRGB8Packed
            );

        if (!allocResult.IsOK())
        {
            qDebug() << "[grab] RGB image alloc failed";
            return false;
        }

        // Bayer → RGB
        PvResult convResult = converter.Convert(buffer, &rgbBuffer);

        if (!convResult.IsOK())
        {
            qDebug() << "[grab] Bayer convert failed";
            return false;
        }

        uint32_t stride =
            rgbImage->GetWidth() * 3 + rgbImage->GetPaddingX();

        QImage qimg(
            rgbImage->GetDataPointer(),
            rgbImage->GetWidth(),
            rgbImage->GetHeight(),
            stride,
            QImage::Format_RGB888
            );

        outImage = qimg.copy();
    }

    else
    {
        qDebug() << "[grab] Unsupported pixel type:"
                 << PvImage::PixelTypeToString(pixelType).GetAscii();
        return false;
    }

    qDebug() << "[grab] PixelType =" << PvImage::PixelTypeToString(pixelType).GetAscii();

    qDebug() << "[grab] Frame OK:" << width << "x" << height;
    return true;
}
// camera_service.cpp
QString CameraService::getCameraModel() const
{
    return m_cameraModel;
}



std::vector<std::string> CameraService::enumerateDevices()
{
    std::vector<std::string> results;

    // ⚠️ 必须是 static，保证 PvDeviceInfo 生命周期
    static PvSystem system;
    system.Find();

    m_selectedDeviceInfo = nullptr;

    for (uint32_t i = 0; i < system.GetInterfaceCount(); ++i)
    {
        const PvInterface* iface = system.GetInterface(i);
        if (!iface) continue;

        for (uint32_t j = 0; j < iface->GetDeviceCount(); ++j)
        {
            const PvDeviceInfo* info = iface->GetDeviceInfo(j);
            if (!info) continue;

            if (m_selectedDeviceInfo == nullptr)
                m_selectedDeviceInfo = info;

            std::ostringstream oss;
            oss << "厂商: " << info->GetVendorName().GetAscii()
                << " | 型号: " << info->GetModelName().GetAscii()
                << " | 序列号: " << info->GetSerialNumber().GetAscii();

            results.push_back(oss.str());
        }
    }

    return results;
}
