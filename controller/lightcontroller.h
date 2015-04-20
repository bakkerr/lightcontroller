#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDebug>

#include "default_values.h"
#include "singlecontroller.h"
#include "audiocontroller.h"
#include "specialbuttons.h"
#include "milightupdsender.h"

class LightController : public QDockWidget
{
    Q_OBJECT

public:
    /* Constructor. */
    explicit LightController(QString title, QString ip, QWidget *parent = 0, bool dummy = 0);

    /* Destructor. */
    ~LightController();

    /* Five zones for each contoller, the controller master and 4 zones. */
    SingleController *zones[5];

    /* Actions to show/hide and create the menu. */
    QMenu *viewControllerMenu;

signals:

public slots:

private slots:
    /* Process the commands to change state and pass then to the UDP Sender. */
    void setBright(unsigned char value, unsigned char zone);
    void setColor(QColor c, unsigned char zone);
    void setOn(unsigned char zone);
    void setOff(unsigned char zone);
    void setWhite(unsigned char zone);

    void contextMenu(const QPoint& x);

private:
    /*
     * See if there are any zones in this controller fixed
     * and should not be changed externally.
     */
    bool areSomeFixed();

    QWidget *m_mainWidget;

    /* The connected UPD sender for this controller. */
    MiLightUPDsender *m_udp;

    /* Main layout */
    QHBoxLayout *m_mainLayout;

};

#endif // LIGHTCONTROLLER_H
