#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include "core/camera_service.h"
#include "core/spectrometer_service.h"
#include <vector>


#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>


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
private slots:
    void on_btnConnectSpectrometer_clicked();
    void on_btnGrabSpectrum_clicked();
    void updateSpectrumPlot(const std::vector<double>& wl,
                            const std::vector<double>& sp);
    void on_btnGrabDark_clicked();
    void on_btnGrabWhite_clicked();
    void updateButtonState();
    void on_btnStartContinuousSpectrum_clicked();
    void on_btnStopSpectrum_clicked();
    void onSpectrumTimerTimeout();
    void on_btnStartTimerspectrum_clicked();

//qt
private:
    Ui::MainWindow *ui;

    QChart*      m_chart;
    QLineSeries* m_series;
    QValueAxis*  m_axisX;
    QValueAxis*  m_axisY;

//相机
private:
    QTimer m_captureTimer;
    QTimer m_videoTimer;     // 连续采集定时器
    QImage m_lastFrame;

    CameraService m_camera;    // 最近一帧（用于显示/resize）
    bool m_isContinuous = false; // 当前是否在连续采集
//光谱仪
private:
    SpectrometerService m_spec;
    std::vector<double> m_wl;          // 波长
    std::vector<double> m_dark;        // 暗参考 D
    std::vector<double> m_white;       // 白参考 W
    std::vector<double> m_sample;      // 样品 S
    bool m_hasDark  = false;
    bool m_hasWhite = false;
    QTimer m_spectrumTimer;   // 连续 / 定时 光谱采集
    enum class SpectrumMode
    {
        Idle,
        Continuous,
        Timed
    };

    SpectrumMode m_spectrumMode = SpectrumMode::Idle;


protected:
    void resizeEvent(QResizeEvent* e) override;

};


