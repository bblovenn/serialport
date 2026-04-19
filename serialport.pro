QT += core gui widgets serialport

CONFIG += c++17
CONFIG += warn_on

TARGET = serialport
TEMPLATE = app

SOURCES += \
    src/core/samplepack.cpp \
    src/core/stream.cpp \
    src/core/streamchannel.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/core/ringbuffer.cpp \
    src/plot/plotwidget.cpp \
    src/readers/abstractreader.cpp \
    src/readers/demoreader.cpp

HEADERS += \
    src/core/ringbuffer.h \
    src/core/samplepack.h \
    src/core/stream.h \
    src/core/streamchannel.h \
    src/mainwindow.h \
    src/plot/plotwidget.h \
    src/readers/abstractreader.h \
    src/readers/demoreader.h

FORMS += \
    src/mainwindow.ui

INCLUDEPATH += \
    src \
    src/core \
    src/readers \
    src/serial \
    src/plot
