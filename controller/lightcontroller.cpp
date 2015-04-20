#include "lightcontroller.h"

LightController::LightController(QString title, QString ip, QWidget *parent, bool dummy) :
    QDockWidget(title, parent)
{
    /* Main Widget. */
    m_mainWidget = new QWidget();
    m_mainWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_mainWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    /* Menu to show/hide the controller/zones. */
    viewControllerMenu = new QMenu(title);

    /* Create a UDP Sender for non-dummy controllers. */
    if(!dummy){
        m_udp = new MiLightUPDsender(this, ip);
    }

    /* The main layout, which contains all zone controllers. */
    m_mainLayout = new QHBoxLayout();

    /* Create 5 zone controllers 1 Master, 4 Zones. */
    for(int i = 0; i <= 4; i++){

        /* Create controller, set title to Master / Zone. */
        if(i == 0){
            zones[i] = new SingleController(tr("Controller Master"), i, this);
        }
        else{
            zones[i] = new SingleController(tr("Zone %0").arg(QString::number(i)), i, this);
        }

        /* Connect signals to UPD Sender if it is not a Dummy. */
        if(!dummy){
            connect(zones[i], SIGNAL(doColor(QColor, unsigned char)), this, SLOT(setColor(QColor, unsigned char)));
            connect(zones[i], SIGNAL(doBright(unsigned char, unsigned char)), this, SLOT(setBright(unsigned char, unsigned char)));
            connect(zones[i], SIGNAL(doOn(unsigned char)), this, SLOT(setOn(unsigned char)));
            connect(zones[i], SIGNAL(doOff(unsigned char)), this, SLOT(setOff(unsigned char)));
            connect(zones[i], SIGNAL(doWhite(unsigned char)), this, SLOT(setWhite(unsigned char)));
        }

        /* For all except the master connect to the Master. */
        if(i != 0){
            connect(zones[0], SIGNAL(doColor(QColor, unsigned char)), zones[i], SLOT(updateColor(QColor)));
            connect(zones[0], SIGNAL(doBright(unsigned char, unsigned char)), zones[i], SLOT(updateBright(unsigned char)));

            connect(zones[0], SIGNAL(doOn(unsigned char)), zones[i], SLOT(updateOn()));
            connect(zones[0], SIGNAL(doOff(unsigned char)), zones[i], SLOT(updateOff()));
            connect(zones[0], SIGNAL(doWhite(unsigned char)), zones[i], SLOT(updateWhite()));

            connect(zones[0], SIGNAL(fadeEnabled()), zones[i], SLOT(disableFade()));
            connect(zones[i], SIGNAL(fadeEnabled()), zones[0], SLOT(disableFade()));
        }

        /* Add the zone to the Menu for show/hide. */
        viewControllerMenu->addAction(zones[i]->viewControllerAction);

        /* Add the zone to the Layout */
        m_mainLayout->addWidget(zones[i]);
    }

    /* Set the layout to the main Widget. */
    m_mainWidget->setLayout(m_mainLayout);

    /* Set the main Widget. */
    setWidget(m_mainWidget);

}

LightController::~LightController()
{

}

void LightController::contextMenu(const QPoint& x)
{
    viewControllerMenu->exec(m_mainWidget->mapToGlobal(x));
}


bool LightController::areSomeFixed()
{
    return (zones[1]->fixed() || zones[2]->fixed() || zones[3]->fixed() || zones[4]->fixed());
}

void LightController::setBright(unsigned char value, unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) m_udp->setBright(value, i);
        }
    }
    else{
        m_udp->setBright(value, zone);
    }
}

void LightController::setColor(QColor c, unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) m_udp->setColor(c, i);
        }
    }
    else{
        m_udp->setColor(c, zone);
    }
}

void LightController::setOn(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) m_udp->setOn(i);
        }
    }
    else{
        m_udp->setOn(zone);
    }
}

void LightController::setOff(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) m_udp->setOff(i);
        }
    }
    else{
        m_udp->setOff(zone);
    }
}

void LightController::setWhite(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!zones[i]->fixed()) m_udp->setWhite(i);
        }
    }
    else{
        m_udp->setWhite(zone);
    }
}
