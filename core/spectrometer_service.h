#pragma once

#include <vector>
#include <string>

// 前置声明，避免头文件污染
namespace oceandirect { namespace api { class OceanDirectAPI; } }

class SpectrometerService
{
public:
    SpectrometerService();
    ~SpectrometerService();

    // 枚举设备（返回设备ID列表也行，这里先给字符串方便显示）
    std::vector<std::string> enumerateDevices();

    // 打开第一个设备（或你传入的 deviceID）
    bool openFirstDevice();
    bool openDevice(long deviceId);

    void close();

    // 取一次光谱：wavelengths 与 spectrum 一一对应
    bool getSpectrumOnce(std::vector<double>& wavelengths,
                         std::vector<double>& spectrum);

    bool isOpen() const { return m_isOpen; }
    long deviceId() const { return m_deviceId; }

private:
    oceandirect::api::OceanDirectAPI* m_api = nullptr; // singleton，不 delete
    long m_deviceId = -1;
    bool m_isOpen = false;
};
