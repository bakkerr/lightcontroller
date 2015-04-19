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
    explicit LightController(QString title, QString ip, QWidget *parent = 0, bool dummy = 0);
    ~LightController();
    SingleController *zones[5];
    QAction *viewControllerAction;
    QMenu *viewControllerMenu;

signals:


public slots:


private slots:
    void setBright(unsigned char value, unsigned char zone);
    void setColor(QColor c, unsigned char zone);
    void setOn(unsigned char zone);
    void setOff(unsigned char zone);
    void setWhite(unsigned char zone);

private:
    bool areSomeFixed();

    MiLightUPDsender *udp;

    QHBoxLayout *layout;

};

#endif // LIGHTCONTROLLER_H
