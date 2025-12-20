#pragma once

class QImage;

class ICamera
{
public:
    virtual ~ICamera() = default;

    virtual bool open() = 0;
    virtual bool grabOnce(QImage& outImage) = 0;
};
