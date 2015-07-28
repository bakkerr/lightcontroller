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
    singlecontroller.cpp \
    milightupdsender.cpp \
    colorwheel.cpp \
    audiocontroller.cpp \
    qcustomplot.cpp \
    mainwindow.cpp \
    fftdisplay.cpp \
    presetcontroller.cpp \
    preset.cpp \
    settings.cpp \
    addcontrollerdialog.cpp \
    container.cpp

HEADERS  += \
    singlecontroller.h \
    milightupdsender.h \
    colorwheel.h \
    audiocontroller.h \
    qcustomplot.h \
    mainwindow.h \
    default_values.h \
    beatcontroller.h \
    fftdisplay.h \
    presetcontroller.h \
    preset.h \
    settings.h \
    addcontrollerdialog.h \
    container.h

LIBS += -L../libbeat/ -lbeat
