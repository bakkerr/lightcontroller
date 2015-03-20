
#include "lightcontroller.h"

LightController::LightController(QWidget *parent) :
    QMainWindow(parent)

{
    setWindowTitle(tr("Light Controller"));
    setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 5px;}");

    udp = new MiLightUPDsender();
    audio = new audioController(this);
    special = new specialButtons(this);

    QWidget *cw = new QWidget(this);

    layout = new QVBoxLayout();

    QHBoxLayout *topLayout = new QHBoxLayout();
    QHBoxLayout *zoneLayout = new QHBoxLayout();

    for(int i = 0; i <= 4; i++){
        zones[i] = new SingleController(i);

        if(i == 0){
            topLayout->addWidget(zones[i], 1);
        }
        else{
            zoneLayout->addWidget(zones[i]);
        }

        connect(zones[i], SIGNAL(colorChange(unsigned char,QColor)), udp, SLOT(setColor(unsigned char,QColor)));
        connect(zones[i], SIGNAL(brightChange(unsigned char,unsigned char)), udp, SLOT(setBright(unsigned char,unsigned char)));
        connect(zones[i], SIGNAL(doOn(unsigned char)), udp, SLOT(setOn(unsigned char)));
        connect(zones[i], SIGNAL(doOff(unsigned char)), udp, SLOT(setOff(unsigned char)));
        connect(zones[i], SIGNAL(doWhite(unsigned char)), udp, SLOT(setWhite(unsigned char)));

        if(i != 0){
            connect(zones[0], SIGNAL(colorChange(unsigned char,QColor)), zones[i], SLOT(setColor(unsigned char,QColor)));
            connect(zones[0], SIGNAL(brightChange(unsigned char,unsigned char)), zones[i], SLOT(setBright(unsigned char,unsigned char)));
            connect(zones[0], SIGNAL(doOn(unsigned char)), zones[i], SLOT(setOn(unsigned char)));
            connect(zones[0], SIGNAL(doOff(unsigned char)), zones[i], SLOT(setOff(unsigned char)));
            connect(zones[0], SIGNAL(doWhite(unsigned char)), zones[i], SLOT(setWhite(unsigned char)));
            connect(zones[0], SIGNAL(fadeEnabled()), zones[i], SLOT(disableFade()));
            connect(zones[i], SIGNAL(fadeEnabled()), zones[0], SLOT(disableFade()));
            connect(special, SIGNAL(allRandom()), zones[i], SLOT(setRandom()));
            connect(special, SIGNAL(allFade()), zones[i], SLOT(enableFade()));
        }
    }

    connect(audio, SIGNAL(beatDetected()), zones[0], SLOT(setRandom()));

    topLayout->addWidget(special, 1);
    topLayout->addWidget(audio, 2);

    layout->addLayout(topLayout, 2);
    layout->addLayout(zoneLayout, 1);

    setCentralWidget(cw);
    cw->setLayout(layout);

}

LightController::~LightController()
{

    //delete ui;
}

