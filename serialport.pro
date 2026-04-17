QT += core gui widgets serialport

CONFIG += c++17
CONFIG += warn_on

TARGET = serialport
TEMPLATE = app

SOURCES += \
    src/core/ringbuffer.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/core/ringbuffer.cpp \
    src/core/samplepack.cpp \
    src/core/stream.cpp \
    src/core/streamchannel.cpp \
    src/readers/abstractreader.cpp \
    src/readers/asciireader.cpp \
    src/readers/demoreader.cpp \
    src/serial/serialcontroller.cpp \
    src/plot/plotwidget.cpp

HEADERS += \
    src/core/ringbuffer.h \
    src/mainwindow.h \
    src/core/samplepack.h \
    src/core/stream.h \
    src/core/streamchannel.h \
    src/readers/abstractreader.h \
    src/readers/asciireader.h \
    src/readers/demoreader.h \
    src/serial/serialcontroller.h \
    src/plot/plotwidget.h

FORMS += \
    src/mainwindow.ui

INCLUDEPATH += \
    src \
    src/core \
    src/readers \
    src/serial \
    src/plot
