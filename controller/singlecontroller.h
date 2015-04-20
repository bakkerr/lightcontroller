#ifndef SINGLECONTROLLER_H
#define SINGLECONTROLLER_H

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include <QAction>

#include <QTimer>

#include "default_values.h"
#include "colorwheel.h"

class SingleController : public QWidget
{
    Q_OBJECT
public:
    /* Constructor. */
    explicit SingleController(QString name, unsigned char z, QWidget *parent = 0);

    /* Destructor. */
    ~SingleController();

    /* View controller to set this controller (in)visible. */
    QAction *viewControllerAction;

    /* Check if this zone is set fixed (it ignores external commands). */
    bool fixed() { return m_fixed; }

signals:
    /* Signals to communicate with the lightcontroller to process the changes to the WiFi Bridge. */
    void doColor(const QColor &color, unsigned char zone);
    void doBright(unsigned char value, unsigned char zone);
    void doWhite(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);
    void fadeEnabled();

public slots:
    /* Manipulate trigger from outside. */
    void setColorExt(const QColor &color);
    void setBrightExt(unsigned char value);
    void setRandomExt();

    void setState(bool state);
    void setOn() { setState(true); }
    void setOff() { setState(false); }
    void setRandom();

    /* Called when state needs to be updated, but is set from a different place. */
    void updateWhite();
    void updateOn();
    void updateOff();

    /* Slots triggered externally. */
    void changeColor(const QColor &color);
    void changeBright(unsigned char value) { setBrightExt(value); emit doBright(value, m_zone); }
    void changeWhite() { doWhite(m_zone); }

    /* Fading Slots. */
    void fade(int n);
    void toggleFade(bool state);
    void enableFade() { toggleFade(true); m_fadeBox->setChecked(true); }
    void disableFade() { toggleFade(false); m_fadeBox->setChecked(false); }
    void setFadeTime(int msec) { m_fadeTimer->setInterval(msec); }

    /* Fading shortcuts. */
    void fade() { fade(1); }
    void fade10() { fade(10); }
    void fade20() { fade(20); }

    /* Color Shortcuts */
    void red() { changeColor(Qt::red); }
    void green() { changeColor(Qt::green); }
    void blue() { changeColor(Qt::blue); }

protected:

private:
    /* Set the layout */
    void setupLayout();
    QHBoxLayout *m_mainLayout;

    /* Private state variables */
    QString m_name;
    unsigned char m_zone;
    bool m_fixed;

    /* Outer groupbox container. */
    QGroupBox *m_groupBox;

    /* Color Picker Wheel. */
    ColorWheel *m_wheel;

    /* Fix this zone (ignore external triggers. */
    QCheckBox *m_fixedBox;

    /* Brightness */
    QLabel *m_brightLabel;
    QSlider *m_brightSlider;

    /* Fading. */
    QCheckBox *m_fadeBox;
    QSlider *m_fadeSlider;
    QTimer *m_fadeTimer;

    /* Shortcut buttons. */
    QPushButton *m_randomButton;
    QPushButton *m_whiteButton;
    QPushButton *m_redButton;
    QPushButton *m_greenButton;
    QPushButton *m_blueButton;

private slots:
    void setFixed(bool s) { m_fixed = s; }
    void color(const QColor c) { emit doColor(c, m_zone); }
    void bright(int value) { emit doBright((unsigned char)value, m_zone); }
    void white() { updateWhite(); emit doWhite(m_zone); }

};

#endif // SINGLECONTROLLER_H
