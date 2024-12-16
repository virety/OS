/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *labelCurrentTemperature;
    QLabel *labelAverageTemperature;
    QPushButton *btnUpdate;
    QWidget *chartWidget;
    QLineEdit *inputPeriod;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        labelCurrentTemperature = new QLabel(centralwidget);
        labelCurrentTemperature->setObjectName(QString::fromUtf8("labelCurrentTemperature"));
        labelCurrentTemperature->setGeometry(QRect(150, 70, 66, 19));
        labelAverageTemperature = new QLabel(centralwidget);
        labelAverageTemperature->setObjectName(QString::fromUtf8("labelAverageTemperature"));
        labelAverageTemperature->setGeometry(QRect(330, 70, 66, 19));
        btnUpdate = new QPushButton(centralwidget);
        btnUpdate->setObjectName(QString::fromUtf8("btnUpdate"));
        btnUpdate->setGeometry(QRect(120, 180, 88, 27));
        chartWidget = new QWidget(centralwidget);
        chartWidget->setObjectName(QString::fromUtf8("chartWidget"));
        chartWidget->setGeometry(QRect(260, 210, 120, 80));
        inputPeriod = new QLineEdit(centralwidget);
        inputPeriod->setObjectName(QString::fromUtf8("inputPeriod"));
        inputPeriod->setGeometry(QRect(210, 130, 113, 27));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        labelCurrentTemperature->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        labelAverageTemperature->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        btnUpdate->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
