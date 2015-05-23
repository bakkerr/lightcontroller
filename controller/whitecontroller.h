#ifndef WHITECONTROLLER_H
#define WHITECONTROLLER_H

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

#include <QInputDialog>

#include <unistd.h>

#include "default_values.h"
#include "settings.h"
#include "colorwheel.h"
#include "preset.h"

class WhiteController : public QWidget
{
    Q_OBJECT
public:
    /* Constructor. */
    explicit WhiteController(QString name, unsigned char z, QWidget *parent = 0);

    /* Destructor. */
    ~WhiteController();

    /* View controller to set this controller (in)visible. */
    QAction *viewControllerAction;

    /* getMethods. */
    QString name()       { return m_name;                   }
    bool    fixed()      { return m_fixed;                  }
    //int     brightness() { return m_brightSlider->value();  }

    PresetZone * getPreset();
    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

signals:
    /* Signals to communicate with the lightcontroller to process the changes to the WiFi Bridge. */
    void doBright(unsigned char value, unsigned char zone);
    void doWhite(unsigned char zone);
    void doOn(unsigned char zone);
    void doOff(unsigned char zone);

public slots:
    void setName(QString name);

    /* Manipulate triggers for zone 0 from Master. */
    void setOnExt();
    void setOffExt();
    void setWhiteExt();
    void setBrightExt(unsigned char value);

    /* Update layout when triggers are from zone 0. */
    void updateOn();
    void updateOff();
    void updateWhite();
    void updateBright(unsigned char value);

    void setPreset(PresetZone *p, bool set);

private slots:
    void setName();

    /* Toggle On/Off */
    void setState(bool state);

    /* Toggle Fixed */
    void setFixed(bool s) { m_fixed = s; }

    /* Internal triggers */
    void setBright(int value)     { emit doBright((unsigned char)value, m_zone);             }
    void setWhite()               { emit doWhite(m_zone); }

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

    /* Fix this zone (ignore external triggers. */
    QCheckBox *m_fixedBox;

    /* Brightness */
    QPushButton *m_brightUpButton;
    QPushButton *m_brightDownButton;
    QPushButton *m_brightFullButton;
    QPushButton *m_warmthUpButton;
    QPushButton *m_warmthDownButton;
    QPushButton *m_nightButton;

};

#endif // WHITECONTROLLER_H
