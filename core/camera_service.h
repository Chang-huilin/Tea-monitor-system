#pragma once

#include "camera_interface.h"

#include <vector>
#include <string>
#include <QString>

// 前置声明，避免头文件污染
class QImage;

class CameraService : public ICamera
{
public:
    CameraService();
    ~CameraService() override;

    bool open() override;
    bool grabOnce(QImage& outImage) override;
    std::vector<std::string> enumerateDevices();
    QString getCameraModel() const;
private:
    void* m_device = nullptr;
    const void* m_selectedDeviceInfo = nullptr;
    QString m_cameraModel;
};
