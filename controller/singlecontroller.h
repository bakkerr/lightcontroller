#ifndef SINGLECONTROLLER_H
#define SINGLECONTROLLER_H

#include <QWidget>

#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include <QAction>

#include <QHBoxLayout>
#include <QVBoxLayout>


#include <QTimer>

#include "default_values.h"
#include "colorwheel.h"

class SingleController : public QWidget
{
    Q_OBJECT
public:
    explicit SingleController(QString name, unsigned char z, QWidget *parent = 0);
    ~SingleController();
    QAction *viewControllerAction;
    bool fixed() { return m_fixed; }

signals:
    void colorChange(const QColor &color, unsigned char zone);
    void brightChange(unsigned char value, unsigned char zone);
    void doWhite(unsigned char zone);
    void doRandom(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);
    void fadeEnabled();

public slots:
    void setColor(const QColor &color);
    void setBright(unsigned char value);

    void setWhite();
    void setRandom();
    void setOn();
    void setOff();


    void changeState(bool state);
    void changeColor(const QColor &color);
    void changeBright(unsigned char value) { setBright(value); emit brightChange(value, zone); }
    void changeWhite() {doWhite(zone); }
    void changeOn() { changeState(true); }
    void changeOff() { changeState(false); }


    void red() { changeColor(Qt::red); }
    void green() { changeColor(Qt::green); }
    void blue() { changeColor(Qt::blue); }

    void fade() { fade(1); }
    void fade10() { fade(10); }
    void fade20() { fade(20); }

    void fade(int n);
    void toggleFade(bool state);
    void enableFade() { toggleFade(true); fadeBox->setChecked(true); }
    void disableFade() { toggleFade(false); fadeBox->setChecked(false); }
    void setFadeTime(int msec) { this->timer->setInterval(msec); }

protected:

private:
    unsigned char zone;
    bool m_fixed;

    QGroupBox *groupbox;

    ColorWheel *wheel;

    QCheckBox *fixedBox;

    QLabel *brightBox;
    QSlider *brightSlider;

    QCheckBox *fadeBox;
    QSlider *fadeSlider;
    QTimer *timer;

    QPushButton *randomButton;
    QPushButton *whiteButton;
    QPushButton *redButton;
    QPushButton *greenButton;
    QPushButton *blueButton;

private slots:
    void setFixed(bool s) { m_fixed = s; }
    void color(const QColor c) { emit colorChange(c, zone); }
    void bright(int value) { emit brightChange((unsigned char)value, zone); }
    void white() { emit doWhite(zone); }

};

#endif // SINGLECONTROLLER_H
