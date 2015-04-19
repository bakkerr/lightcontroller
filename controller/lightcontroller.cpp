#include "lightcontroller.h"

LightController::LightController(QString title, QString ip, QWidget *parent, bool dummy) :
    QDockWidget(title, parent)
{
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    QWidget *mw = new QWidget();

    viewControllerMenu = new QMenu(title);

    viewControllerAction = new QAction(title, this);
    viewControllerAction->setCheckable(true);
    viewControllerAction->setChecked(true);
    connect(viewControllerAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));

    viewControllerMenu->addAction(viewControllerAction);
    viewControllerMenu->addSeparator();

    if(!dummy){
        udp = new MiLightUPDsender(this, ip);
    }

    layout = new QHBoxLayout();

    for(int i = 0; i <= 4; i++){
        if(i == 0){
            zones[i] = new SingleController(tr("Controller Master"), i, this);
        }
        else{
            zones[i] = new SingleController(tr("Zone %0").arg(QString::number(i)), i, this);
        }

        if(!dummy){
            connect(zones[i], SIGNAL(doColor(QColor, unsigned char)), this, SLOT(setColor(QColor, unsigned char)));
            connect(zones[i], SIGNAL(doBright(unsigned char, unsigned char)), this, SLOT(setBright(unsigned char, unsigned char)));
            connect(zones[i], SIGNAL(doOn(unsigned char)), this, SLOT(setOn(unsigned char)));
            connect(zones[i], SIGNAL(doOff(unsigned char)), this, SLOT(setOff(unsigned char)));
            connect(zones[i], SIGNAL(doWhite(unsigned char)), this, SLOT(setWhite(unsigned char)));
        }

        if(i != 0){
            connect(zones[0], SIGNAL(doColor(QColor, unsigned char)), zones[i], SLOT(setColorExt(QColor)));
            connect(zones[0], SIGNAL(doBright(unsigned char, unsigned char)), zones[i], SLOT(setBrightExt(unsigned char)));

            connect(zones[0], SIGNAL(doOn(unsigned char)), zones[i], SLOT(updateOn()));
            connect(zones[0], SIGNAL(doOff(unsigned char)), zones[i], SLOT(updateOff()));
            connect(zones[0], SIGNAL(doWhite(unsigned char)), zones[i], SLOT(updateWhite()));

            connect(zones[0], SIGNAL(fadeEnabled()), zones[i], SLOT(disableFade()));
            connect(zones[i], SIGNAL(fadeEnabled()), zones[0], SLOT(disableFade()));
        }

        layout->addWidget(zones[i]);
        viewControllerMenu->addAction(zones[i]->viewControllerAction);
    }

    mw->setLayout(layout);

    setWidget(mw);
}

LightController::~LightController()
{

}

bool LightController::areSomeFixed()
{
    return (zones[1]->fixed() || zones[2]->fixed() || zones[3]->fixed() || zones[4]->fixed());
}

void LightController::setBright(unsigned char value, unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) udp->setBright(value, i);
        }
    }
    else{
        udp->setBright(value, zone);
    }
}

void LightController::setColor(QColor c, unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) udp->setColor(c, i);
        }
    }
    else{
        udp->setColor(c, zone);
    }
}

void LightController::setOn(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) udp->setOn(i);
        }
    }
    else{
        udp->setOn(zone);
    }
}

void LightController::setOff(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) udp->setOff(i);
        }
    }
    else{
        udp->setOff(zone);
    }
}

void LightController::setWhite(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) udp->setWhite(i);
        }
    }
    else{
        udp->setWhite(zone);
    }
}
