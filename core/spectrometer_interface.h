#pragma once

#include <vector>
#include <string>

class ISpectrometer
{
public:
    virtual ~ISpectrometer() = default;

    // 枚举光谱仪（返回可读描述）
    virtual std::vector<std::string> enumerateDevices() = 0;

    // 打开第一个光谱仪
    virtual bool openFirstDevice() = 0;

    // 打开指定 index 的光谱仪
    virtual bool openDevice(int deviceIndex) = 0;

    // 关闭
    virtual void close() = 0;

    // 单次采集光谱
    virtual bool getSpectrumOnce(
        std::vector<double>& wavelengths,
        std::vector<double>& spectrum) = 0;

    // 是否已打开
    virtual bool isOpen() const = 0;
};
