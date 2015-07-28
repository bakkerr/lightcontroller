#ifndef SINGLECONTROLLER_H
#define SINGLECONTROLLER_H

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

class SingleController : public QWidget
{
    Q_OBJECT
public:
    /* Constructor. */
    explicit SingleController(quint16 id, QString name, quint16 remote, QList<SingleController *> slaves, quint16 containerID, QWidget *parent = 0);

    /* Destructor. */
    ~SingleController();

    /* View controller to set this controller (in)visible. */
    QAction *viewControllerAction;

    /* getMethods. */
    quint16 id()         { return m_id;                     }
    QString name()       { return m_name;                   }
    quint16 remote()     { return m_zone;                   }
    quint16 container()  { return m_container;              }
    bool    fixed()      { return m_fixed;                  }
    bool    fadeTime()   { return m_fadeSlider->value();    }
    QColor  color()      { return m_wheel->color();         }
    int     brightness() { return m_brightSlider->value();  }

    QList<SingleController*> slaves() { return m_slaves; }

    bool areSlavesFixed();

    /* Settings */
    PresetLC * getPreset();
    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

signals:
    /* Signals to communicate with the UDP controller. */
    void doOn(quint16 zone);
    void doOff(quint16 zone);
    void doColor(const QColor &color, quint16 zone);
    void doBright(unsigned char value, quint16 zone);
    void doWhite(quint16 zone);
    void doNight(quint16 zone);

    void doBuildinEffect(quint16 zone);
    void doIncreaseSpeed(quint16 zone);
    void doDecreaseSpeed(quint16 zone);

    void doPair(quint16 zone);
    void doUnPair(quint16 zone);

    /* Sync fade state between Master and Zones. */
    void fadeEnabled();

public slots:
    void setName(QString name);

    /* Manipulate triggers for zone 0 from Master. */
    void setOnExt();
    void setOffExt();
    void setColorExt(const QColor &color);
    void setBrightExt(unsigned char value);
    void setWhiteExt();
    void setNightExt();

    void setRandomExt(bool same = true);
    void setRandomExtSame()       { setRandomExt(true);  }
    void setRandomExtDifferent()  { setRandomExt(false); }
    void fadeExt()   { if(!m_fixed) fade(1);  }
    void fade10Ext() { if(!m_fixed) fade(10); }
    void fade20Ext() { if(!m_fixed) fade(20); }

    /* Update layout when triggers are from zone 0. */
    void updateOn();
    void updateOff();
    void updateColor(const QColor &color);
    void updateBright(unsigned char value);
    void updateWhite();
    void updateNight();


    /* Fading Slots. */
    void enableFade();
    void disableFade();

    void setPreset(PresetLC *p, bool set);

private slots:
    void setName();

    /* Toggle Fixed */
    void setFixed(bool s) { m_fixed = s; }

    /* Toggle On/Off */
    void setState(bool state);

    /* Internal triggers */
    void setColor(const QColor &color);
    void setBright(int value);
    void setWhite();
    void setNight();

    void setBuildinEffect();
    void decreaseSpeed();
    void increaseSpeed();

    /* Pairing slots. */
    void pair()   { emit(doPair(m_zone));   }
    void unPair() { emit(doUnPair(m_zone)); }

    void setRandom(bool same = true);

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

    /* Other Effects */
    void flash();
    void flashRandom() { setWhite(); setRandom(); }

    void contextMenu(const QPoint& x);

private:
    /* Set the layout */
    void setupLayout();

    /* Private state variables */
    quint16 m_id;
    QString m_name;
    quint16 m_zone;
    quint16 m_container;
    bool m_state;
    bool m_fixed;
    QList<SingleController*> m_slaves;


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

#endif // SINGLECONTROLLER_H
