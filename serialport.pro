QT += core gui widgets serialport

CONFIG += c++17
CONFIG += warn_on

TARGET = serialport
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/core/ringbuffer.cpp \
    src/core/samplepack.cpp \
    src/core/stream.cpp \
    src/core/streamchannel.cpp \
    src/protocol/protocolframe.cpp \
    src/protocol/protocolparser.cpp \
    src/readers/abstractreader.cpp \
    src/readers/asciireader.cpp \
    src/readers/demoreader.cpp \
    src/serial/serialcontroller.cpp \
    src/storage/csvrecorder.cpp \
    src/plot/plotwidget.cpp
    

HEADERS += \
    src/mainwindow.h \
    src/core/ringbuffer.h \
    src/core/samplepack.h \
    src/core/stream.h \
    src/core/streamchannel.h \
    src/protocol/protocolframe.h \
    src/protocol/protocolparser.h \
    src/readers/abstractreader.h \
    src/readers/asciireader.h \
    src/readers/demoreader.h \
    src/serial/serialcontroller.h \
    src/storage/csvrecorder.h \
    src/plot/plotwidget.h

FORMS += \
    src/mainwindow.ui

RESOURCES += \
    resources.qrc

INCLUDEPATH += \
    src \
    src/core \
    src/protocol \
    src/readers \
    src/serial \
    src/storage \
    src/plot
