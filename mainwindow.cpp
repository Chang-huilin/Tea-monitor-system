#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QPixmap>

// QtCharts (Qt6)
#include <QtCharts>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <algorithm>
#include <limits>
#include <cmath>


static QString ensureDir(const QString& subDir)
{
    const QString base =
        QCoreApplication::applicationDirPath() + "/captures/" + subDir;

    QDir dir;
    if (!dir.exists(base))
        dir.mkpath(base);

    return base;
}

static QString makeFilename(const QString& prefix)
{
    return prefix + "_" +
           QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz") +
           ".png";
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet(R"(
QMainWindow, QWidget {
    background-color: #dfe4ec;
    color: #1f2d3d;
    font-family: "SimHei", "Times New Roman";
    font-size: 10pt;
}

QMenuBar {
    background-color: #4f86e8;
    color: white;
    spacing: 8px;
    border: none;
    min-height: 34px;
}

QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
}

QMenuBar::item:selected {
    background: rgba(255,255,255,0.18);
    border-radius: 4px;
}

QMenu {
    background-color: #f4f7fb;
    border: 1px solid #b9c3d3;
}

QMenu::item {
    padding: 6px 24px 6px 18px;
}

QMenu::item:selected {
    background-color: #d9e8ff;
}

QPushButton {
    background-color: #dbe6f7;
    border: 1px solid #b8c7dc;
    border-radius: 4px;
    padding: 8px 14px;
    min-height: 34px;
    color: #19345f;
}

QPushButton:hover {
    background-color: #cfe0f8;
    border: 1px solid #9db6da;
}

QPushButton:pressed {
    background-color: #bdd3f3;
}

QPushButton:disabled {
    background-color: #e8edf5;
    color: #8b97a8;
    border: 1px solid #c8d1de;
}

QLabel {
    color: #24364f;
    background: transparent;
}

QGroupBox {
    border: 1px solid #b8c1cf;
    margin-top: 10px;
    padding-top: 10px;
    background-color: #e6ebf3;
    font-weight: bold;
    color: #23395d;
}

QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px 0 4px;
}

QLineEdit, QPlainTextEdit, QTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
    background-color: #f8fafc;
    border: 1px solid #b8c1cf;
    border-radius: 3px;
    padding: 4px 6px;
    selection-background-color: #4f86e8;
    selection-color: white;
}

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 22px;
    border-left: 1px solid #b8c1cf;
    background-color: #e8eef8;
}

QComboBox QAbstractItemView {
    background-color: #f8fafc;
    border: 1px solid #b8c1cf;
    selection-background-color: #d8e7ff;
}

QPlainTextEdit, QTextEdit {
    background-color: white;
}

QFrame, QLabel#imageView, QWidget#spectrumView {
    border: 1px solid #b8c1cf;
}

QLabel#imageView {
    background-color: #142845;
}

QWidget#spectrumView {
    background-color: #142845;
}

QTabWidget::pane {
    border: 1px solid #b8c1cf;
    background: #eef2f7;
    top: -1px;
}

QTabBar::tab {
    background: #dbe6f7;
    border: 1px solid #b8c1cf;
    padding: 6px 14px;
    margin-right: 2px;
    color: #1f3761;
}

QTabBar::tab:selected {
    background: #4f86e8;
    color: white;
}

QHeaderView::section {
    background-color: #dbe6f7;
    color: #1f3761;
    padding: 6px;
    border: 1px solid #b8c1cf;
    font-weight: bold;
}

QTableWidget, QTableView {
    background-color: white;
    gridline-color: #c8d1de;
    border: 1px solid #b8c1cf;
}

QScrollBar:vertical {
    background: #e7ebf1;
    width: 12px;
    margin: 0;
}

QScrollBar::handle:vertical {
    background: #b7c5d8;
    min-height: 20px;
    border-radius: 4px;
}

QScrollBar:horizontal {
    background: #e7ebf1;
    height: 12px;
    margin: 0;
}

QScrollBar::handle:horizontal {
    background: #b7c5d8;
    min-width: 20px;
    border-radius: 4px;
}

QStatusBar {
    background-color: #e8edf5;
    border-top: 1px solid #b8c1cf;
    color: #31445f;
}
)");
    // 日志：只读 + 限制行数，防止长期运行卡顿
    ui->logOutput->setReadOnly(true);
    ui->logOutput->document()->setMaximumBlockCount(200); // 最多2000行，自动丢弃最老的

    // imageView 显示策略
    ui->imageView->setScaledContents(false);
    ui->imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 连续采集定时器（例如 20 FPS）
    m_videoTimer.setInterval(50);
    connect(&m_videoTimer, &QTimer::timeout, this, &MainWindow::onVideoTick);

    // 定时抓拍（60 秒）
    m_captureTimer.setInterval(60 * 1000);
    connect(&m_captureTimer, &QTimer::timeout, this, &MainWindow::onCaptureTimeout);

    // ==========================
    // 光谱图初始化（Qt6 需要 QtCharts::）
    // ==========================
    m_series = new QLineSeries(this);

    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("Spectrum");

    // X：波长
    m_axisX = new QValueAxis();
    m_axisX->setTitleText("Wavelength (nm)");
    m_axisX->setLabelFormat("%.0f");

    // Y：强度
    m_axisY = new QValueAxis();
    m_axisY->setTitleText("Intensity");
    m_axisY->setLabelFormat("%.0f");

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);

    // 放进 UI（spectrumView 必须是 QWidget 容器）
    auto* chartView = new QChartView(m_chart, ui->spectrumView);
    chartView->setRenderHint(QPainter::Antialiasing, true);

    // 避免重复 setLayout：如果 ui 里已经给 spectrumView 配了 layout，这里就不要再 new
    if (!ui->spectrumView->layout())
    {
        auto* layout = new QVBoxLayout(ui->spectrumView);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(chartView);
    }
    else
    {
        ui->spectrumView->layout()->setContentsMargins(0, 0, 0, 0);
        ui->spectrumView->layout()->addWidget(chartView);
    }
    updateButtonState();
    // 光谱采集定时器
    m_spectrumTimer.setSingleShot(false);
    connect(&m_spectrumTimer, &QTimer::timeout,
            this, &MainWindow::onSpectrumTimerTimeout);

    updateButtonState();
    QString onnxPath = QCoreApplication::applicationDirPath() + "/wither_3class_multimodal.onnx";
    if (!m_infer.init(onnxPath)) {
        ui->logOutput->appendPlainText("ONNX init failed: " + onnxPath);
    } else {
        ui->logOutput->appendPlainText("ONNX init ok: " + onnxPath);
    };
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =======================
// 连接相机
// =======================
void MainWindow::on_btnConnect_clicked()
{
    ui->logOutput->appendPlainText("开始寻找相机设备...");

    auto devices = m_camera.enumerateDevices();
    if (devices.empty())
    {
        ui->logOutput->appendPlainText("未发现任何相机设备");
        return;
    }

    if (m_camera.open())
    {
        const QString model = m_camera.getCameraModel();

        ui->logOutput->appendPlainText("相机打开成功，型号：" + model);
        ui->statusbar->showMessage("已连接：" + model);
    }
    else
    {
        ui->logOutput->appendPlainText("相机打开失败");
    }
    updateButtonState();

}

// =======================
// 手动拍摄模式
// =======================
void MainWindow::on_btnGrab_clicked()
{
    QImage img;
    if (!m_camera.grabOnce(img))
    {
        ui->logOutput->appendPlainText("手动拍摄失败");
        return;
    }

    ui->imageView->setPixmap(
        QPixmap::fromImage(img).scaled(
            ui->imageView->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));

    // 保存
    const QString dir = ensureDir("manual");
    const QString filename = dir + "/" + makeFilename("manual");

    if (img.save(filename))
        ui->logOutput->appendPlainText("手动拍摄已保存：" + filename);
    else
        ui->logOutput->appendPlainText("手动拍摄保存失败");
}

// =======================
// 定时拍照模式
// =======================
void MainWindow::on_btnStartTimercamera_clicked()
{
    if (!m_captureTimer.isActive())
    {
        m_captureTimer.start();
        ui->logOutput->appendPlainText("已启动 60 秒定时抓拍");
        ui->statusbar->showMessage("定时抓拍运行中");
    }
}

void MainWindow::onCaptureTimeout()
{
    QImage img;
    if (!m_camera.grabOnce(img))
    {
        ui->logOutput->appendPlainText("定时拍摄失败");
        return;
    }

    ui->imageView->setPixmap(
        QPixmap::fromImage(img).scaled(
            ui->imageView->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));

    // 保存
    const QString dir = ensureDir("timer");
    const QString filename = dir + "/" + makeFilename("timer");

    if (img.save(filename))
        ui->logOutput->appendPlainText("定时拍摄已保存：" + filename);
    else
        ui->logOutput->appendPlainText("定时拍摄保存失败");
}

// =======================
// 视频模式
// =======================
void MainWindow::on_btnStartContinuous_clicked()
{
    if (!m_isContinuous)
    {
        if (!m_videoTimer.isActive())
            m_videoTimer.start();

        m_isContinuous = true;
        ui->btnStartContinuous->setText("停止连续拍摄");
        ui->logOutput->appendPlainText("连续拍摄已启动");
        ui->statusbar->showMessage("连续采集中");
    }
    else
    {
        m_videoTimer.stop();
        m_isContinuous = false;
        ui->btnStartContinuous->setText("开始连续拍摄");
        ui->logOutput->appendPlainText("连续拍摄已停止");
        ui->statusbar->showMessage("连续采集已停止");
    }
}

void MainWindow::onVideoTick()
{
    QImage img;
    if (m_camera.grabOnce(img))
    {
        m_lastFrame = img;
        ui->imageView->setPixmap(
            QPixmap::fromImage(img).scaled(
                ui->imageView->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
    }
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QMainWindow::resizeEvent(e);

    if (!m_lastFrame.isNull())
    {
        ui->imageView->setPixmap(
            QPixmap::fromImage(m_lastFrame).scaled(
                ui->imageView->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
    }
}

// =======================
// 光谱仪功能
// =======================
//连接光谱仪
void MainWindow::on_btnConnectSpectrometer_clicked()
{
    ui->logOutput->appendPlainText("开始寻找光谱仪...");

    auto devs = m_spec.enumerateDevices();
    if (devs.empty())
    {
        ui->logOutput->appendPlainText("未发现光谱仪");
        return;
    }

    ui->logOutput->appendPlainText(QString::fromStdString(devs[0]));

    if (m_spec.openFirstDevice()){
        ui->logOutput->appendPlainText("光谱仪打开成功");
        QString model =
            QString::fromStdString(m_spec.spectrometerModel());
        QString manu  =
            QString::fromStdString(m_spec.spectrometerManufacturer());

        ui->logOutput->appendPlainText(
            QString("光谱仪信息：%1 %2")
                .arg(manu,model)
        );
        updateButtonState();
    }
    else
        ui->logOutput->appendPlainText("光谱仪打开失败");


}


void MainWindow::updateButtonState()
{
    bool connected = m_spec.isOpen();
    bool running   = (m_spectrumMode != SpectrumMode::Idle);

    ui->btnConnectSpectrometer->setEnabled(!connected);

    ui->btnGrabDark->setEnabled(connected && !running);
    ui->btnGrabWhite->setEnabled(connected && m_hasDark && !running);
    ui->btnGrabSpectrum->setEnabled(connected && !running);

    ui->btnStartContinuousSpectrum->setEnabled(connected && !running);
    ui->btnStartTimer->setEnabled(connected && !running);
    ui->btnStopSpectrum->setEnabled(running);
}


//采集数据
void MainWindow::on_btnGrabSpectrum_clicked()
{
    std::vector<double> wl, sp;
    const int N = 5;
    if (!m_spec.getSpectrumOnce(wl, sp))
    {
        ui->logOutput->appendPlainText("光谱采集失败");
        return;
    }

    ui->logOutput->appendPlainText(
        QString("光谱采集成功：%1 points").arg((int)sp.size()));

    m_wl     = wl;
    m_sample = sp;

    ui->logOutput->appendPlainText(
        QString("样品采集成功：%1 points，平均 %2 次").arg((int)sp.size()).arg(N));
    // ⭐ 关键：采集后更新图
    updateSpectrumPlot(wl, sp);
}

// N 次平均采集：返回平均光谱（spAvg），波长 wl 使用第一次采集的 wl
static bool captureAverageSpectrum(
    SpectrometerService& spec,
    int N,
    std::vector<double>& wlOut,
    std::vector<double>& spAvgOut)
{
    if (N <= 0) N = 1;

    std::vector<double> wl, sp;
    std::vector<double> sum;

    for (int k = 0; k < N; ++k)
    {
        wl.clear(); sp.clear();
        if (!spec.getSpectrumOnce(wl, sp))
            return false;

        if (k == 0)
        {
            wlOut = wl;
            sum.assign(sp.size(), 0.0);
        }
        else
        {
            // 点数不一致直接失败（防止设备状态变化）
            if (sp.size() != sum.size())
                return false;
        }

        for (size_t i = 0; i < sp.size(); ++i)
            sum[i] += sp[i];
    }

    spAvgOut.resize(sum.size());
    for (size_t i = 0; i < sum.size(); ++i)
        spAvgOut[i] = sum[i] / static_cast<double>(N);

    return true;
}

//计算反射率的函数
static std::vector<double> computeReflectance(
    const std::vector<double>& sample,
    const std::vector<double>& white,
    const std::vector<double>& dark)
{
    const size_t n = sample.size();
    std::vector<double> refl(n, std::numeric_limits<double>::quiet_NaN());

    // 工程阈值：分母至少达到白参考峰值的某个比例
    const auto maxWhiteIt = std::max_element(white.begin(), white.end());
    const double maxWhite = (maxWhiteIt != white.end()) ? *maxWhiteIt : 0.0;

    // 经验值：2%~10% 都常用；先用 5%
    const double denomMin = 0.05 * maxWhite;

    for (size_t i = 0; i < n; ++i)
    {
        const double denom = white[i] - dark[i];

        // 分母太小：该点无效（置 NaN，后面绘图断线）
        if (denom <= denomMin)
            continue;

        const double r = (sample[i] - dark[i]) / denom;

        // 这里不做 0/1 裁剪；只做一个温和的物理范围过滤（可选）
        // 允许一点点超出，避免边界抖动被硬切
        if (r < -0.1 || r > 1.5)
            continue;

        refl[i] = r;  // 0~1（反射率）
    }

    return refl;
}


//绘图
void MainWindow::updateSpectrumPlot(const std::vector<double>& wl,
                                    const std::vector<double>& sp)
{
    if (wl.empty() || sp.empty() || wl.size() != sp.size()
        || !m_series || !m_axisX || !m_axisY)
        return;

    bool hasCal = (m_hasDark && m_hasWhite
                   && m_dark.size() == sp.size()
                   && m_white.size() == sp.size());

    std::vector<double> y = sp;

    if (hasCal)
    {
        y = computeReflectance(sp, m_white, m_dark);

        m_axisY->setTitleText("Reflectance");
        m_axisY->setLabelFormat("%.3f");
        m_axisY->setRange(0.0, 1.0);

        ui->logOutput->appendPlainText("显示：反射率谱");
    }
    else
    {
        m_axisY->setTitleText("Intensity");
        m_axisY->setLabelFormat("%.0f");

        auto [minIt, maxIt] = std::minmax_element(y.begin(), y.end());
        m_axisY->setRange(*minIt, *maxIt);

        ui->logOutput->appendPlainText("显示：原始强度谱（未标定）");
    }

    QVector<QPointF> points;
    points.reserve(static_cast<int>(wl.size()));

    for (size_t i = 0; i < wl.size(); ++i)
    {
        const double yy = y[i];
        if (std::isnan(yy))
        {
            // QtCharts 对 NaN 点通常会断开折线（形成缺口）
            points.append(QPointF(wl[i], std::numeric_limits<double>::quiet_NaN()));
        }
        else
        {
            points.append(QPointF(wl[i], yy));
        }
    }
    m_series->replace(points);

    m_axisX->setRange(wl.front(), wl.back());
}


//采暗参考
void MainWindow::on_btnGrabDark_clicked()
{
    std::vector<double> wl, sp;
    const int N = 10;

    if (!captureAverageSpectrum(m_spec, N, wl, sp))
    {
        ui->logOutput->appendPlainText("暗参考采集失败");
        return;
    }

    m_wl = wl;
    m_dark = sp;
    m_hasDark = true;

    ui->logOutput->appendPlainText(
        QString("暗参考采集完成：%1 points，平均 %2 次").arg((int)sp.size()).arg(N));

    updateButtonState();
}


void MainWindow::on_btnGrabWhite_clicked()
{
    if (!m_hasDark)
    {
        ui->logOutput->appendPlainText("请先采集暗参考");
        return;
    }

    std::vector<double> wl, sp;
    const int N = 10;

    if (!captureAverageSpectrum(m_spec, N, wl, sp))
    {
        ui->logOutput->appendPlainText("白参考采集失败");
        return;
    }

    if (sp.size() != m_dark.size())
    {
        ui->logOutput->appendPlainText("白参考与暗参考点数不一致");
        return;
    }

    m_wl = wl;
    m_white = sp;
    m_hasWhite = true;

    // 简单质量提示：分母太小的点比例
    int bad = 0;
    const double maxWhite = *std::max_element(m_white.begin(), m_white.end());
    const double denomMin = 0.05 * maxWhite;

    for (size_t i = 0; i < sp.size(); ++i)
        if ((m_white[i] - m_dark[i]) <= denomMin) bad++;

    ui->logOutput->appendPlainText(
        QString("白参考采集完成：%1 points，平均 %2 次").arg((int)sp.size()).arg(N));
    ui->logOutput->appendPlainText(
        QString("白参考有效性：分母过小点 %1 / %2").arg(bad).arg((int)sp.size()));

    updateButtonState();
}


//采集函数
void MainWindow::onSpectrumTimerTimeout()
{
    if (!m_spec.isOpen())
        return;

    // 模式保护
    if (m_spectrumMode == SpectrumMode::Idle)
        return;

    std::vector<double> wl, sp;

    // 连续/定时都做平均
    const int N =
        (m_spectrumMode == SpectrumMode::Continuous) ? 3 : 5;

    if (!captureAverageSpectrum(m_spec, N, wl, sp))
    {
        ui->logOutput->appendPlainText("光谱采集失败");
        return;
    }

    m_wl     = wl;
    m_sample = sp;

    updateSpectrumPlot(m_wl, m_sample);

    if (m_spectrumMode == SpectrumMode::Timed)
    {
        ui->logOutput->appendPlainText("定时采集：完成一次光谱");
    }
}


//连续采集按钮（实时光谱）
void MainWindow::on_btnStartContinuousSpectrum_clicked()
{
    if (!m_spec.isOpen())
        return;

    m_spectrumMode = SpectrumMode::Continuous;

    // 连续采集周期（例如 200 ms）
    m_spectrumTimer.start(200);

    ui->logOutput->appendPlainText("开始连续光谱采集");
    updateButtonState();
}

//定时采集
void MainWindow::on_btnStartTimerspectrum_clicked()
{
    if (!m_spec.isOpen())
        return;

    m_spectrumMode = SpectrumMode::Timed;

    // 60 秒
    m_spectrumTimer.start(60 * 1000);

    ui->logOutput->appendPlainText("开始定时光谱采集（60 s）");
    updateButtonState();
}

//停止采集
void MainWindow::on_btnStopSpectrum_clicked()
{
    m_spectrumTimer.stop();
    m_spectrumMode = SpectrumMode::Idle;

    ui->logOutput->appendPlainText("停止光谱采集");
    updateButtonState();
}
