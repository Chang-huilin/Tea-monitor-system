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
    QPushButton *btnConnect;
    QPlainTextEdit *logOutput;
    QLabel *imageView;
    QPushButton *btnGrab;
    QPushButton *btnStartTimer;
    QPushButton *btnStartContinuous;
    QPushButton *pushButton;
    QLabel *spectralView;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1386, 779);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnConnect = new QPushButton(centralwidget);
        btnConnect->setObjectName("btnConnect");
        btnConnect->setGeometry(QRect(130, 10, 161, 81));
        logOutput = new QPlainTextEdit(centralwidget);
        logOutput->setObjectName("logOutput");
        logOutput->setEnabled(true);
        logOutput->setGeometry(QRect(10, 580, 1301, 141));
        imageView = new QLabel(centralwidget);
        imageView->setObjectName("imageView");
        imageView->setGeometry(QRect(90, 200, 471, 341));
        imageView->setMaximumSize(QSize(640, 480));
        imageView->setFrameShape(QFrame::Shape::Box);
        imageView->setScaledContents(false);
        btnGrab = new QPushButton(centralwidget);
        btnGrab->setObjectName("btnGrab");
        btnGrab->setGeometry(QRect(130, 110, 161, 81));
        btnStartTimer = new QPushButton(centralwidget);
        btnStartTimer->setObjectName("btnStartTimer");
        btnStartTimer->setGeometry(QRect(370, 110, 151, 81));
        btnStartContinuous = new QPushButton(centralwidget);
        btnStartContinuous->setObjectName("btnStartContinuous");
        btnStartContinuous->setGeometry(QRect(370, 10, 151, 81));
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(820, 50, 141, 71));
        spectralView = new QLabel(centralwidget);
        spectralView->setObjectName("spectralView");
        spectralView->setGeometry(QRect(810, 200, 471, 341));
        spectralView->setMaximumSize(QSize(640, 480));
        spectralView->setFrameShape(QFrame::Shape::Box);
        spectralView->setScaledContents(false);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1386, 26));
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
        imageView->setText(QString());
        btnGrab->setText(QCoreApplication::translate("MainWindow", "\346\211\213\345\212\250\346\213\215\346\221\204", nullptr));
        btnStartTimer->setText(QCoreApplication::translate("MainWindow", "\345\256\232\346\227\266\346\213\215\346\221\204", nullptr));
        btnStartContinuous->setText(QCoreApplication::translate("MainWindow", "\350\207\252\345\212\250\346\213\215\346\221\204", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\350\277\236\346\216\245\345\205\211\350\260\261\344\273\252", nullptr));
        spectralView->setText(QString());
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "Edit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
