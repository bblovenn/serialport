cmake_minimum_required(VERSION 3.16)

project(serialport VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort)

add_executable(serialport
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/core/ringbuffer.cpp
    src/core/ringbuffer.h
    src/core/samplepack.cpp
    src/core/samplepack.h
    src/core/stream.cpp
    src/core/stream.h
    src/core/streamchannel.cpp
    src/core/streamchannel.h
    src/protocol/protocolframe.cpp
    src/protocol/protocolframe.h
    src/protocol/protocolparser.cpp
    src/protocol/protocolparser.h
    src/readers/abstractreader.cpp
    src/readers/abstractreader.h
    src/readers/asciireader.cpp
    src/readers/asciireader.h
    src/readers/demoreader.cpp
    src/readers/demoreader.h
    src/serial/serialcontroller.cpp
    src/serial/serialcontroller.h
    src/storage/csvrecorder.cpp
    src/storage/csvrecorder.h
    src/plot/plotwidget.cpp
    src/plot/plotwidget.h
    resources.qrc
)

target_include_directories(serialport PRIVATE
    src
    src/core
    src/protocol
    src/readers
    src/serial
    src/storage
    src/plot
)

target_link_libraries(serialport PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)
