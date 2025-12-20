#include "spectrometer_service.h"

#include <api/OceanDirectAPI.h>   // 关键：正确路径
#include <QDebug>

using oceandirect::api::OceanDirectAPI;

SpectrometerService::SpectrometerService()
{
    // singleton
    m_api = OceanDirectAPI::getInstance();
}

SpectrometerService::~SpectrometerService()
{
    close();

    // 关键：不要 delete m_api
    // SDK 要求用 shutdown 释放 singleton
    OceanDirectAPI::shutdown();
    m_api = nullptr;
}

std::vector<std::string> SpectrometerService::enumerateDevices()
{
    std::vector<std::string> out;
    if (!m_api) return out;

    int err = 0;

    // 1) 探测设备
    m_api->probeDevices();

    // 2) 取 deviceIDs
    int n = m_api->getNumberOfDeviceIDs();
    if (n <= 0)
        return out;

    std::vector<long> ids(n);
    m_api->getDeviceIDs(ids.data(), n);

    // 先简单输出：ID
    for (auto id : ids)
    {
        out.push_back("Spectrometer deviceID = " + std::to_string(id));
    }

    return out;
}

bool SpectrometerService::openFirstDevice()
{
    auto list = enumerateDevices();
    if (list.empty()) return false;

    // 重新取一遍真实 id（避免你以后字符串格式变了）
    int n = m_api->getNumberOfDeviceIDs();
    if (n <= 0) return false;
    std::vector<long> ids(n);
    m_api->getDeviceIDs(ids.data(), n);

    return openDevice(ids[0]);
}

bool SpectrometerService::openDevice(long deviceId)
{
    if (!m_api) return false;

    int err = 0;

    // 已打开就先关
    if (m_isOpen)
        close();

    // OceanDirect: openDevice(long deviceID, int* errorCode)
    m_api->openDevice(deviceId, &err);
    if (err != 0)
    {
        qDebug() << "[spec] openDevice failed, err =" << err;
        m_isOpen = false;
        m_deviceId = -1;
        return false;
    }

    m_deviceId = deviceId;
    m_isOpen = true;

    qDebug() << "[spec] openDevice OK, deviceId =" << m_deviceId;
    return true;
}

void SpectrometerService::close()
{
    if (!m_api || !m_isOpen) return;

    int err = 0;
    m_api->closeDevice(m_deviceId, &err);

    qDebug() << "[spec] closeDevice err =" << err;

    m_isOpen = false;
    m_deviceId = -1;
}

bool SpectrometerService::getSpectrumOnce(std::vector<double>& wavelengths,
                                          std::vector<double>& spectrum)
{
    wavelengths.clear();
    spectrum.clear();

    if (!m_api || !m_isOpen) return false;

    int err = 0;

    // 用 formatted spectrum（double），最省事
    int len = m_api->getFormattedSpectrumLength(m_deviceId, &err);
    if (err != 0 || len <= 0)
    {
        qDebug() << "[spec] getFormattedSpectrumLength failed, err =" << err << "len =" << len;
        return false;
    }

    wavelengths.resize(len);
    spectrum.resize(len);

    // wavelengths
    int wlCopied = m_api->getWavelengths(m_deviceId, &err, wavelengths.data(), len);
    if (err != 0 || wlCopied <= 0)
    {
        qDebug() << "[spec] getWavelengths failed, err =" << err << "copied =" << wlCopied;
        return false;
    }

    // spectrum
    int spCopied = m_api->getFormattedSpectrum(m_deviceId, &err, spectrum.data(), len);
    if (err != 0 || spCopied <= 0)
    {
        qDebug() << "[spec] getFormattedSpectrum failed, err =" << err << "copied =" << spCopied;
        return false;
    }

    return true;
}
