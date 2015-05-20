#ifndef RGBCONTROLLER_H
#define RGBCONTROLLER_H

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

#include <QMenu>

#include <QAction>

#include <QTimer>

#include <QInputDialog>

#include <unistd.h>

#include "default_values.h"
#include "settings.h"
#include "colorwheel.h"
#include "preset.h"

class RGBController : public QWidget
{
    Q_OBJECT
public:
    /* Constructor. */
    explicit RGBController(QString name, unsigned char z, QWidget *parent = 0);

    /* Destructor. */
    ~RGBController();

    /* View controller to set this controller (in)visible. */
    QAction *viewControllerAction;

    /* getMethods. */
    QString name()       { return m_name;                   }
    bool    fixed()      { return m_fixed;                  }
    bool    fadeTime()   { return m_fadeSlider->value();    }
    QColor  color()      { return m_wheel->color();         }
    int     brightness() { return m_brightSlider->value();  }

    PresetZone * getPreset();
    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

signals:
    /* Signals to communicate with the lightcontroller to process the changes to the WiFi Bridge. */
    void doColor(const QColor &color, unsigned char zone);
    void doBright(unsigned char value, unsigned char zone);
    void doWhite(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);
    void doBuildinEffect(unsigned char zone);
    void doIncreaseSpeed(unsigned char zone);
    void doDecreaseSpeed(unsigned char zone);

    /* Sync fade state between Master and Zones. */
    void fadeEnabled();

public slots:
    void setName(QString name);

    /* Manipulate triggers for zone 0 from Master. */
    void setOnExt();
    void setOffExt();
    void setColorExt(const QColor &color);
    void setWhiteExt();
    void setBrightExt(unsigned char value);
    void setRandomExt();
    void fadeExt()   { if(!m_fixed) fade(1);  }
    void fade10Ext() { if(!m_fixed) fade(10); }
    void fade20Ext() { if(!m_fixed) fade(20); }

    /* Update layout when triggers are from zone 0. */
    void updateOn();
    void updateOff();
    void updateColor(const QColor &color);
    void updateWhite();
    void updateBright(unsigned char value);

    /* Fading Slots. */
    void enableFade();
    void disableFade();

    void setPreset(PresetZone *p, bool set);

private slots:
    void setName();

    /* Toggle On/Off */
    void setState(bool state);

    /* Toggle Fixed */
    void setFixed(bool s) { m_fixed = s; }

    /* Internal triggers */
    void setColor(const QColor c) { emit doColor(c, m_zone);                                 }
    void setBright(int value)     { emit doBright((unsigned char)value, m_zone);             }
    void setWhite()               { m_wheel->setInnerColor(Qt::white); emit doWhite(m_zone); }
    void setBuildinEffect()       { emit doBuildinEffect(m_zone);                            }
    void decreaseSpeed()          { emit doDecreaseSpeed(m_zone);                            }
    void increaseSpeed()          { emit doIncreaseSpeed(m_zone);                            }
    void setRandom();

    /* Color Shortcuts */
    void red()   { if(m_wheel->color() != Qt::red)   m_wheel->changeColor(Qt::red);   }
    void green() { if(m_wheel->color() != Qt::green) m_wheel->changeColor(Qt::green); }
    void blue()  { if(m_wheel->color() != Qt::blue)  m_wheel->changeColor(Qt::blue);  }

    /* Fading. */
    void fade(int n);
    void toggleFade(bool state);
    void setFadeTime(int msec);

    /* Fading shortcuts. */
    void fade()   { fade(1);  }
    void fade10() { fade(10); }
    void fade20() { fade(20); }

    void contextMenu(const QPoint& x);

private:
    /* Set the layout */
    void setupLayout();
    QHBoxLayout *m_mainLayout;

    /* Private state variables */
    QString m_name;
    unsigned char m_zone;
    bool m_state;
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

    /* Buildin Effect Buttons. */
    QPushButton *m_buildinEffectButton;
    QPushButton *m_incSpeedButton;
    QPushButton *m_decSpeedButton;

    /* Shortcut buttons. */
    QPushButton *m_randomButton;
    QPushButton *m_whiteButton;
    QPushButton *m_redButton;
    QPushButton *m_greenButton;
    QPushButton *m_blueButton;

};

#endif // RGBCONTROLLER_H
