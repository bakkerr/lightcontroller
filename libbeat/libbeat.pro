TEMPLATE = lib
LANGUAGE = C++
TARGET   = beat

QT += multimedia

SOURCES += \
    subband.cpp \
    soundbuffer.cpp \
    fft.cpp \
    beatcontroller.cpp \
    beatanalyser.cpp

HEADERS += subband.h \
    soundbuffer.h \
    fft.h \
    beatcontroller.h \
    beatanalyser.h

LIBS += -lfftw3
target.path = /usr/lib
INSTALLS += target
