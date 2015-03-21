#ifndef SINGLECONTROLLER_H
#define SINGLECONTROLLER_H

#include <QWidget>
#include <QtGui>

#include "default_values.h"
#include "colorwheel.h"

class SingleController : public QWidget
{
    Q_OBJECT
public:
    explicit SingleController(unsigned char z, QWidget *parent = 0);
    ~SingleController();

signals:
    void colorChange(unsigned char zone, const QColor &color);
    void brightChange(unsigned char zone, unsigned char value);
    void doWhite(unsigned char zone);
    void doRandom(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);
    void fadeEnabled();

public slots:
    void changeState(bool state);
    void changeColor(const QColor &color);
    void setColor(const QColor &color);
    void setColor(unsigned char zone, const QColor &color) { setColor(color); }
    void setBright(unsigned char value);
    void setBright(unsigned char zone, unsigned char value) { setBright(value); }
    void setWhite();
    void setWhite(unsigned char zone) { setWhite(); }
    void setRandom();
    void setOn();
    void setOn(unsigned char zone) { setOn(); }
    void setOff();
    void setOff(unsigned char zone) { setOff(); }

    void red() { changeColor(Qt::red); }
    void green() { changeColor(Qt::green); }
    void blue() { changeColor(Qt::blue); }

    void fade();
    void toggleFade(bool state);
    void enableFade() { toggleFade(true); fadeBox->setChecked(true); }
    void disableFade() { toggleFade(false); fadeBox->setChecked(false); }
    void setFadeTime(int msec) { this->timer->setInterval(msec); }

protected:

private:
    unsigned char zone;

    QGroupBox *groupbox;

    ColorWheel *wheel;

    QGroupBox *brightBox;
    QSlider *brightSlider;

    QGroupBox *fadeBox;
    QSlider *fadeSlider;
    QTimer *timer;

    QGroupBox *buttonBox;
    QPushButton *randomButton;
    QPushButton *whiteButton;
    QPushButton *redButton;
    QPushButton *greenButton;
    QPushButton *blueButton;

private slots:
    void color(const QColor c) { emit colorChange(zone, c); }
    void bright(int value) { emit brightChange(zone, (unsigned char)value); }
    void white() { emit doWhite(zone); }

};

#endif // SINGLECONTROLLER_H
