#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QDateTime>
#include <QSizePolicy>

#include <QDir>
#include <QDateTime>

static QString ensureDir(const QString& subDir)
{
    QString base = QCoreApplication::applicationDirPath()
    + "/captures/" + subDir;

    QDir dir;
    if (!dir.exists(base))
        dir.mkpath(base);

    return base;
}

static QString makeFilename(const QString& prefix)
{
    return prefix + "_" +
           QDateTime::currentDateTime()
               .toString("yyyyMMdd_HHmmss_zzz") +
           ".png";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // imageView 显示策略
    ui->imageView->setScaledContents(false);
    ui->imageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 连续采集定时器（例如 20 FPS）
    m_videoTimer.setInterval(50);
    connect(&m_videoTimer, &QTimer::timeout,
            this, &MainWindow::onVideoTick);

    m_captureTimer.setInterval(60 * 1000);

    connect(&m_captureTimer, &QTimer::timeout,
            this, &MainWindow::onCaptureTimeout);
}



MainWindow::~MainWindow()
{
    delete ui;
}
//连接相机
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
        QString model = m_camera.getCameraModel();

        ui->logOutput->appendPlainText(
            "相机打开成功，型号：" + model
            );

        ui->statusbar->showMessage(
            "已连接：" + model
            );
    }
    else
    {
        ui->logOutput->appendPlainText("相机打开失败");
    }
}
//手动拍摄模式
void MainWindow::on_btnGrab_clicked()
{
    QImage img;
    if (!m_camera.grabOnce(img))
    {
        ui->logOutput->appendPlainText("手动拍摄失败");
        return;
    }

    // 显示
    ui->imageView->setPixmap(
        QPixmap::fromImage(img).scaled(
            ui->imageView->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );

    // 保存
    QString dir = ensureDir("manual");
    QString filename = dir + "/" + makeFilename("manual");

    if (img.save(filename))
        ui->logOutput->appendPlainText("手动拍摄已保存：" + filename);
    else
        ui->logOutput->appendPlainText("手动拍摄保存失败");
}


//定时拍照模式
void MainWindow::on_btnStartTimer_clicked()
{
    if (!m_captureTimer.isActive())
    {
        m_captureTimer.start();
        ui->logOutput->appendPlainText("已启动 60 秒定时抓拍");
        ui->statusbar->showMessage("定时抓拍运行中");
    }
}
//定时拍摄保存
void MainWindow::onCaptureTimeout()
{
    QImage img;
    if (!m_camera.grabOnce(img))
    {
        ui->logOutput->appendPlainText("定时拍摄失败");
        return;
    }

    // 显示
    ui->imageView->setPixmap(
        QPixmap::fromImage(img).scaled(
            ui->imageView->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            )
        );

    // 保存
    QString dir = ensureDir("timer");
    QString filename = dir + "/" + makeFilename("timer");

    if (img.save(filename))
        ui->logOutput->appendPlainText("定时拍摄已保存：" + filename);
    else
        ui->logOutput->appendPlainText("定时拍摄保存失败");
}

//视频模式
void MainWindow::on_btnStartContinuous_clicked()
{
    if (!m_isContinuous)
    {
        // ===== 开始连续采集 =====
        if (!m_videoTimer.isActive())
            m_videoTimer.start();

        m_isContinuous = true;
        ui->btnStartContinuous->setText("停止连续拍摄");
        ui->logOutput->appendPlainText("连续拍摄已启动");
        ui->statusbar->showMessage("连续采集中");
    }
    else
    {
        // ===== 停止连续采集 =====
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

        QPixmap pm = QPixmap::fromImage(img);
        ui->imageView->setPixmap(
            pm.scaled(ui->imageView->size(),
                      Qt::KeepAspectRatio,
                      Qt::SmoothTransformation)
            );
    }
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    QMainWindow::resizeEvent(e);

    if (!m_lastFrame.isNull())
    {
        QPixmap pm = QPixmap::fromImage(m_lastFrame);
        ui->imageView->setPixmap(
            pm.scaled(ui->imageView->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
}
// ===== 光谱仪功能部分=====
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

    if (m_spec.openFirstDevice())
    {
        ui->logOutput->appendPlainText("光谱仪打开成功");
    }
    else
    {
        ui->logOutput->appendPlainText("光谱仪打开失败");
    }
}


void MainWindow::on_btnGrabSpectrum_clicked()
{
    std::vector<double> wl, sp;
    if (!m_spec.getSpectrumOnce(wl, sp))
    {
        ui->logOutput->appendPlainText("光谱采集失败");
        return;
    }

    ui->logOutput->appendPlainText(
        QString("光谱采集成功：%1 points").arg((int)sp.size())
        );

    // 先简单打印前5个点，确认数据是真的
    for (int i = 0; i < 5 && i < (int)sp.size(); ++i)
    {
        ui->logOutput->appendPlainText(
            QString("wl=%1 nm, sp=%2").arg(wl[i]).arg(sp[i])
            );
    }
}
