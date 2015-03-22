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
    explicit LightController(QString title, QWidget *parent = 0, bool dummy = 0);
    ~LightController();
    SingleController *zones[5];

signals:


public slots:


private:
    MiLightUPDsender *udp;

    QHBoxLayout *layout;


};

#endif // LIGHTCONTROLLER_H
