/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *btnConnect;
    QPushButton *btnGrab;
    QPushButton *btnStartContinuous;
    QPushButton *btnStartTimer;
    QPushButton *btnConnectSpectrometer;
    QPushButton *btnStopSpectrum;
    QPushButton *btnGrabDark;
    QPushButton *btnGrabWhite;
    QPushButton *btnGrabSpectrum;
    QPushButton *btnStartContinuousSpectrum;
    QPushButton *btnStartTimerspectrum;
    QLabel *imageView;
    QWidget *spectrumView;
    QPlainTextEdit *logOutput;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 900);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setHorizontalSpacing(18);
        gridLayout_2->setVerticalSpacing(14);
        gridLayout_2->setContentsMargins(18, 18, 18, 18);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(12);
        gridLayout->setVerticalSpacing(12);
        btnConnect = new QPushButton(centralwidget);
        btnConnect->setObjectName("btnConnect");
        btnConnect->setMinimumSize(QSize(180, 52));
        QFont font;
        font.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font.setPointSize(11);
        font.setBold(true);
        btnConnect->setFont(font);

        gridLayout->addWidget(btnConnect, 0, 0, 1, 2);

        btnGrab = new QPushButton(centralwidget);
        btnGrab->setObjectName("btnGrab");
        btnGrab->setMinimumSize(QSize(180, 52));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Microsoft YaHei")});
        font1.setPointSize(11);
        btnGrab->setFont(font1);

        gridLayout->addWidget(btnGrab, 1, 0, 1, 1);

        btnStartContinuous = new QPushButton(centralwidget);
        btnStartContinuous->setObjectName("btnStartContinuous");
        btnStartContinuous->setMinimumSize(QSize(180, 52));
        btnStartContinuous->setFont(font1);

        gridLayout->addWidget(btnStartContinuous, 1, 1, 1, 1);

        btnStartTimer = new QPushButton(centralwidget);
        btnStartTimer->setObjectName("btnStartTimer");
        btnStartTimer->setMinimumSize(QSize(180, 52));
        btnStartTimer->setFont(font);

        gridLayout->addWidget(btnStartTimer, 2, 0, 1, 2);

        btnConnectSpectrometer = new QPushButton(centralwidget);
        btnConnectSpectrometer->setObjectName("btnConnectSpectrometer");
        btnConnectSpectrometer->setMinimumSize(QSize(180, 52));
        btnConnectSpectrometer->setFont(font);

        gridLayout->addWidget(btnConnectSpectrometer, 3, 0, 1, 1);

        btnStopSpectrum = new QPushButton(centralwidget);
        btnStopSpectrum->setObjectName("btnStopSpectrum");
        btnStopSpectrum->setMinimumSize(QSize(180, 52));
        btnStopSpectrum->setFont(font1);

        gridLayout->addWidget(btnStopSpectrum, 3, 1, 1, 1);

        btnGrabDark = new QPushButton(centralwidget);
        btnGrabDark->setObjectName("btnGrabDark");
        btnGrabDark->setMinimumSize(QSize(180, 52));
        btnGrabDark->setFont(font1);

        gridLayout->addWidget(btnGrabDark, 4, 0, 1, 1);

        btnGrabWhite = new QPushButton(centralwidget);
        btnGrabWhite->setObjectName("btnGrabWhite");
        btnGrabWhite->setMinimumSize(QSize(180, 52));
        btnGrabWhite->setFont(font1);

        gridLayout->addWidget(btnGrabWhite, 4, 1, 1, 1);

        btnGrabSpectrum = new QPushButton(centralwidget);
        btnGrabSpectrum->setObjectName("btnGrabSpectrum");
        btnGrabSpectrum->setMinimumSize(QSize(180, 52));
        btnGrabSpectrum->setFont(font1);

        gridLayout->addWidget(btnGrabSpectrum, 5, 0, 1, 1);

        btnStartContinuousSpectrum = new QPushButton(centralwidget);
        btnStartContinuousSpectrum->setObjectName("btnStartContinuousSpectrum");
        btnStartContinuousSpectrum->setMinimumSize(QSize(180, 52));
        btnStartContinuousSpectrum->setFont(font1);

        gridLayout->addWidget(btnStartContinuousSpectrum, 5, 1, 1, 1);

        btnStartTimerspectrum = new QPushButton(centralwidget);
        btnStartTimerspectrum->setObjectName("btnStartTimerspectrum");
        btnStartTimerspectrum->setMinimumSize(QSize(180, 52));
        btnStartTimerspectrum->setFont(font);

        gridLayout->addWidget(btnStartTimerspectrum, 6, 0, 1, 2);


        gridLayout_2->addLayout(gridLayout, 0, 0, 2, 1);

        imageView = new QLabel(centralwidget);
        imageView->setObjectName("imageView");
        imageView->setMinimumSize(QSize(700, 380));
        imageView->setFrameShape(QFrame::Shape::Box);
        imageView->setLineWidth(1);
        imageView->setScaledContents(false);

        gridLayout_2->addWidget(imageView, 0, 1, 1, 1);

        spectrumView = new QWidget(centralwidget);
        spectrumView->setObjectName("spectrumView");
        spectrumView->setMinimumSize(QSize(700, 280));

        gridLayout_2->addWidget(spectrumView, 1, 1, 1, 1);

        logOutput = new QPlainTextEdit(centralwidget);
        logOutput->setObjectName("logOutput");
        logOutput->setEnabled(true);
        logOutput->setMinimumSize(QSize(0, 140));
        logOutput->setMaximumSize(QSize(16777215, 180));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Consolas")});
        font2.setPointSize(10);
        logOutput->setFont(font2);

        gridLayout_2->addWidget(logOutput, 2, 0, 1, 2);

        MainWindow->setCentralWidget(centralwidget);
        spectrumView->raise();
        logOutput->raise();
        imageView->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 26));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName("menuEdit");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnConnect->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\347\233\270\346\234\272", nullptr));
        btnGrab->setText(QCoreApplication::translate("MainWindow", "\346\211\213\345\212\250\346\213\215\346\221\204\357\274\210\345\215\225\345\274\240\357\274\211", nullptr));
        btnStartContinuous->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\213\215\346\221\204", nullptr));
        btnStartTimer->setText(QCoreApplication::translate("MainWindow", "\345\256\232\346\227\266\346\213\215\346\221\204", nullptr));
        btnConnectSpectrometer->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\345\205\211\350\260\261\344\273\252", nullptr));
        btnStopSpectrum->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        btnGrabDark->setText(QCoreApplication::translate("MainWindow", "Dark\351\207\207\346\232\227", nullptr));
        btnGrabWhite->setText(QCoreApplication::translate("MainWindow", "White", nullptr));
        btnGrabSpectrum->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\345\215\225\346\254\241\345\205\211\350\260\261", nullptr));
        btnStartContinuousSpectrum->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\350\277\236\347\273\255\345\205\211\350\260\261", nullptr));
        btnStartTimerspectrum->setText(QCoreApplication::translate("MainWindow", "\345\256\232\346\227\266\351\207\207\351\233\206", nullptr));
        imageView->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
