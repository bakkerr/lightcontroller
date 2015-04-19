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
    void doColor(const QColor &color, unsigned char zone);
    void doBright(unsigned char value, unsigned char zone);
    void doWhite(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);
    void fadeEnabled();

public slots:
    void setColorExt(const QColor &color);
    void setBrightExt(unsigned char value);
    void setRandomExt();

    void setRandom();

    void updateWhite();
    void updateOn();
    void updateOff();

    void changeState(bool state);
    void changeColor(const QColor &color);
    void changeBright(unsigned char value) { setBrightExt(value); emit doBright(value, m_zone); }
    void changeWhite() { doWhite(m_zone); }
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
    void enableFade() { toggleFade(true); m_fadeBox->setChecked(true); }
    void disableFade() { toggleFade(false); m_fadeBox->setChecked(false); }
    void setFadeTime(int msec) { m_fadeTimer->setInterval(msec); }

protected:

private:
    unsigned char m_zone;
    bool m_fixed;

    QGroupBox *m_groupbox;

    ColorWheel *m_wheel;

    QCheckBox *m_fixedBox;

    QLabel *m_brightBox;
    QSlider *m_brightSlider;

    QCheckBox *m_fadeBox;
    QSlider *m_fadeSlider;
    QTimer *m_fadeTimer;

    QPushButton *m_randomButton;
    QPushButton *m_whiteButton;
    QPushButton *m_redButton;
    QPushButton *m_greenButton;
    QPushButton *m_blueButton;

private slots:
    void setFixed(bool s) { m_fixed = s; }
    void color(const QColor c) { emit doColor(c, m_zone); }
    void bright(int value) { emit doBright((unsigned char)value, m_zone); }
    void white() { emit doWhite(m_zone); }

};

#endif // SINGLECONTROLLER_H
