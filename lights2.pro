#-------------------------------------------------
#
# Project created by QtCreator 2015-03-10T23:56:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lights2
TEMPLATE = app


SOURCES += main.cpp\
        lightcontroller.cpp \
    singlecontroller.cpp \
    milightupdsender.cpp \
    colorwheel.cpp \
    audiocontroller.cpp \
    specialbuttons.cpp \
    qcustomplot.cpp

HEADERS  += lightcontroller.h \
    singlecontroller.h \
    milightupdsender.h \
    colorwheel.h \
    audiocontroller.h \
    specialbuttons.h \
    qcustomplot.h \
    config.h

FORMS    += lightcontroller.ui
