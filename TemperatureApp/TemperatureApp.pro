QT += core gui network
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TemperatureApp
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui
QTDIR = /path/to/your/qt
INCLUDEPATH += $$QTDIR/include
LIBS += -L$$QTDIR/lib
