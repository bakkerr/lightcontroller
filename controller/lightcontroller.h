#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QMainWindow>
#include <QDockWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>

#include <QInputDialog>

#include "default_values.h"
#include "settings.h"
#include "singlecontroller.h"
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
    SingleController *zones[5];

    /* Actions to show/hide and create the menu. */
    QMenu *viewControllerMenu;

signals:

public slots:
    void setPreset(PresetLC *p);
    void setName(QString name);

private slots:
    void setName();

    /* Process the commands to change state and pass then to the UDP Sender. */
    void setBright(unsigned char value, unsigned char zone);
    void setColor(QColor c, unsigned char zone);
    void setOn(unsigned char zone);
    void setOff(unsigned char zone);
    void setWhite(unsigned char zone);
    void setBuildinEffect(unsigned char zone);
    void setIncreaseSpeed(unsigned char zone);
    void setDecreaseSpeed(unsigned char zone);

    void contextMenu(const QPoint& x);

private:
    /*
     * See if there are any zones in this controller fixed
     * and should not be changed externally.
     */
    bool areSomeFixed();

    int m_num;
    QString m_id;
    QString m_ip;
    QString m_name;

    QWidget *m_mainWidget;

    /* The connected UPD sender for this controller. */
    MiLightUPDsender *m_udp;

    /* Main layout */
    QHBoxLayout *m_mainLayout;

};

#endif // LIGHTCONTROLLER_H
