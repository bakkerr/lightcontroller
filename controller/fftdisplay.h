#ifndef FFTDISPLAY_H
#define FFTDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QAudioInput>

#include "cmath"
#include "../libbeat/beatcontroller.h"

class FFTDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit FFTDisplay(libbeat::BeatController *controller, QWidget *parent = 0);

public slots:
    void drawNewData();
    void setController(libbeat::BeatController *controller) { m_Controller = controller; }

private:
    libbeat::BeatController *m_Controller;

protected:
    void paintEvent(QPaintEvent *);

};

#endif // FFTDISPLAY_H
