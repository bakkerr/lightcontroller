#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QMainWindow>
#include <QDockWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>

#include <QToolBar>
#include <QToolButton>

#include <QInputDialog>

#include "default_values.h"
#include "settings.h"
#include "rgbcontroller.h"
#include "whitecontroller.h"
#include "preset.h"
#include "milightupdsender.h"

class LightController : public QDockWidget
{
    Q_OBJECT

public:
    /* Constructor. */
    explicit LightController(QString ip, QString id, int num = -1, bool dummy = 0, QWidget *parent = 0);

    /* Destructor. */
    ~LightController();

    QString id() { return m_id; }

    PresetLC *getPreset();
    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    /* Five zones for each contoller, the controller master and 4 zones. */
    RGBController *RGBzones[5];
    WhiteController *whiteZones[5];

    /* Actions to show/hide and create the menu. */
    QMenu *viewControllerMenu;

signals:

public slots:
    void setPreset(PresetLC *p);
    void setName(QString name);

private slots:
    void setName();

    /* Process the commands to change state and pass then to the UDP Sender. */
    void RGBSetBright(unsigned char value, unsigned char zone);
    void RGBSetColor(QColor c, unsigned char zone);
    void RGBSetOn(unsigned char zone);
    void RGBSetOff(unsigned char zone);
    void RGBSetWhite(unsigned char zone);
    void RGBSetBuildinEffect(unsigned char zone);
    void RGBIncreaseSpeed(unsigned char zone);
    void RGBDecreaseSpeed(unsigned char zone);

    void whiteSetOn(unsigned char zone);
    void whiteSetOff(unsigned char zone);
    void whiteSetNight(unsigned char zone);
    void whiteIncreaseBright(unsigned char zone);
    void whiteDecreaseBright(unsigned char zone);

    void contextMenu(const QPoint& x);

private:
    /*
     * See if there are any zones in this controller fixed
     * and should not be changed externally.
     */
    bool RGBAreSomeFixed();
    bool whiteAreSomeFixed();

    int m_num;
    QString m_id;
    QString m_ip;
    QString m_name;

    QWidget *m_mainWidget;
    QWidget *m_rgbWidget;
    QWidget *m_whiteWidget;

    QAction *m_viewRGBAction;
    QAction *m_viewWhiteAction;

    /* The connected UPD sender for this controller. */
    MiLightUPDsender *m_udp;

    /* Main layout */
    QVBoxLayout *m_mainLayout;

};

#endif // LIGHTCONTROLLER_H
