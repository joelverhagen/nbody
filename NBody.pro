QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NBody
TEMPLATE = app

SOURCES += main.cpp\
    widget.cpp \
    nbodypanel.cpp \
    nbodythread.cpp \
    nbodyni.cpp \
    nbodyref.cpp \
    basenbody.cpp \
    nbodymg.cpp

HEADERS  += widget.h \
    nbodypanel.h \
    nbodythread.h \
    nbodyni.h \
    nbodyref.h \
    basenbody.h \
    nbodymg.h

FORMS    += widget.ui

INCLUDEPATH += "C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.0\\include"
LIBS += -lOpenCL -L"C:\\Program Files\\NVIDIA GPU Computing Toolkit\\CUDA\\v5.0\\lib\\x64"
