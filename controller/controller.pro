#-------------------------------------------------
#
# Project created by QtCreator 2015-03-10T23:56:54
#
#-------------------------------------------------

QT       += core gui network printsupport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lightcontroller
TEMPLATE = app

DEPENDPATH  += ../libbeat
INCLUDEPATH += ../libbeat

SOURCES += main.cpp\
        lightcontroller.cpp \
    singlecontroller.cpp \
    milightupdsender.cpp \
    colorwheel.cpp \
    audiocontroller.cpp \
    specialbuttons.cpp \
    qcustomplot.cpp \
    mainwindow.cpp \
    discover.cpp \
    fftdisplay.cpp

HEADERS  += lightcontroller.h \
    singlecontroller.h \
    milightupdsender.h \
    colorwheel.h \
    audiocontroller.h \
    specialbuttons.h \
    qcustomplot.h \
    discover.h \
    mainwindow.h \
    default_values.h \
    beatcontroller.h \
    fftdisplay.h

LIBS += -L../libbeat/ -lbeat