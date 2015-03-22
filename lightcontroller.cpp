#include "lightcontroller.h"

LightController::LightController(QString title, QWidget *parent, bool dummy) :
    QDockWidget(title, parent)
{
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *mw = new QWidget();

    if(!dummy){
        udp = new MiLightUPDsender();
    }

    layout = new QHBoxLayout();

    //QSpacerItem *sp1 = new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding);
    //QSpacerItem *sp2 = new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding);

    for(int i = 0; i <= 4; i++){
        zones[i] = new SingleController(i, this);

        layout->addWidget(zones[i]);
        //if(i == 0) layout->addSpacerItem(sp1);

        if(!dummy){

            connect(zones[i], SIGNAL(colorChange(QColor, unsigned char)), udp, SLOT(setColor(QColor, unsigned char)));
            connect(zones[i], SIGNAL(brightChange(unsigned char, unsigned char)), udp, SLOT(setBright(unsigned char, unsigned char)));
            connect(zones[i], SIGNAL(doOn(unsigned char)), udp, SLOT(setOn(unsigned char)));
            connect(zones[i], SIGNAL(doOff(unsigned char)), udp, SLOT(setOff(unsigned char)));
            connect(zones[i], SIGNAL(doWhite(unsigned char)), udp, SLOT(setWhite(unsigned char)));

            if(i != 0){
                connect(zones[0], SIGNAL(colorChange(QColor, unsigned char)), zones[i], SLOT(setColor(QColor)));
                connect(zones[0], SIGNAL(brightChange(unsigned char, unsigned char)), zones[i], SLOT(setBright(unsigned char)));
                connect(zones[0], SIGNAL(doOn(unsigned char)), zones[i], SLOT(setOn()));
                connect(zones[0], SIGNAL(doOff(unsigned char)), zones[i], SLOT(setOff()));
                connect(zones[0], SIGNAL(doWhite(unsigned char)), zones[i], SLOT(setWhite()));
                connect(zones[0], SIGNAL(fadeEnabled()), zones[i], SLOT(disableFade()));
                connect(zones[i], SIGNAL(fadeEnabled()), zones[0], SLOT(disableFade()));

            }
        }
    }

    mw->setLayout(layout);

    setWidget(mw);
}

LightController::~LightController()
{

}

