QT += core gui widgets serialport

CONFIG += c++17
CONFIG += warn_on

TARGET = serialport
TEMPLATE = app

SOURCES += \
    src/core/samplepack.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/core/ringbuffer.cpp

HEADERS += \
    src/core/ringbuffer.h \
    src/core/samplepack.h \
    src/mainwindow.h

FORMS += \
    src/mainwindow.ui

INCLUDEPATH += \
    src \
    src/core \
    src/readers \
    src/serial \
    src/plot
