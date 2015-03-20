#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

#include "config.h"
#include "singlecontroller.h"
#include "audiocontroller.h"
#include "specialbuttons.h"
#include "milightupdsender.h"

class LightController : public QMainWindow
{
    Q_OBJECT

public:
    explicit LightController(QWidget *parent = 0);
    ~LightController();
    SingleController *zones[5];

signals:

public slots:

private:
    MiLightUPDsender *udp;

    QWidget *centralWidget;
    QVBoxLayout *layout;
    audioController *audio;
    specialButtons *special;

};

#endif // LIGHTCONTROLLER_H
