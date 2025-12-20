#pragma once

#include <QMainWindow>
#include <QTimer>
#include "core/camera_service.h"
#include "core/spectrometer_service.h"
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();
    void on_btnStartTimer_clicked();   // 新增
    void onCaptureTimeout();           // 定时槽
    void on_btnGrab_clicked();
    void on_btnStartContinuous_clicked(); // 连续拍摄按钮
    void onVideoTick();                   // 定时抓帧
    void on_btnConnectSpectrometer_clicked();
    void on_btnGrabSpectrum_clicked();
private:
    Ui::MainWindow *ui;
    CameraService m_camera;
    QTimer m_captureTimer;
    QTimer m_videoTimer;     // 连续采集定时器
    QImage m_lastFrame;      // 最近一帧（用于显示/resize）
    bool m_isContinuous = false; // 当前是否在连续采集
private:
    SpectrometerService m_spec;
protected:
    void resizeEvent(QResizeEvent* e) override;

};
