#include "lightcontroller.h"

LightController::LightController(QString ip, QString id, int num, bool dummy, QWidget *parent) :
    QDockWidget(id, parent)
{
    m_id = id;
    m_ip = ip;
    m_num = num;
    m_name = tr(LC_DEFAULT_NAME) + tr(" ") + QString::number(m_num);

    /* Main Widget. */
    m_mainWidget = new QWidget();
    m_mainWidget->setContentsMargins(0, 0, 0, 0);
    m_mainWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_mainWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    /* Menu to show/hide the controller/zones. */
    viewControllerMenu = new QMenu(m_name, this);
    viewControllerMenu->addAction(toggleViewAction());
    viewControllerMenu->addSeparator();
    QAction *idAction = viewControllerMenu->addAction(tr("ID: ") + m_id);
    idAction->setEnabled(false);
    QAction *ipAction = viewControllerMenu->addAction(tr("IP: ") + m_ip);
    ipAction->setEnabled(false);
    viewControllerMenu->addSeparator();
    QAction *setNameAction = viewControllerMenu->addAction(tr("Change name"));
    connect(setNameAction, SIGNAL(triggered()), this, SLOT(setName()));
    viewControllerMenu->addSeparator();

    /* Create a UDP Sender for non-dummy controllers. */
    if(!dummy){
        m_udp = new MiLightUPDsender(ip, MILIGHT_PORT_DEFAULT, this);
    }

    m_mainLayout = new QVBoxLayout();

    m_rgbWidget = new QWidget(this);
    m_rgbWidget->setContentsMargins(0, 0, 0, 0);
    m_viewRGBAction = new QAction(tr("RGB"), m_rgbWidget);
    m_viewRGBAction->setCheckable(true);
    m_viewRGBAction->setChecked(true);
    connect(m_viewRGBAction, SIGNAL(toggled(bool)), m_rgbWidget, SLOT(setVisible(bool)));
    viewControllerMenu->addAction(m_viewRGBAction);
    QHBoxLayout *rgbLayout = new QHBoxLayout();

    /* Create 5 zone controllers 1 Master, 4 Zones. */
    for(int i = 0; i <= 4; i++){

        /* Create controller, set title to Master / Zone. */
        if(i == 0){
            RGBzones[i] = new RGBController(tr(LC_RGB_MASTER_NAME), i, this);
        }
        else{
            RGBzones[i] = new RGBController(tr(LC_RGB_ZONE_NAME) + tr(" ") + QString::number(i), i, this);
        }

        /* Connect signals to UPD Sender if it is not a Dummy. */
        if(!dummy){
            connect(RGBzones[i], SIGNAL(doColor(QColor, unsigned char)), this, SLOT(setColor(QColor, unsigned char)));
            connect(RGBzones[i], SIGNAL(doBright(unsigned char, unsigned char)), this, SLOT(setBright(unsigned char, unsigned char)));
            connect(RGBzones[i], SIGNAL(doOn(unsigned char)), this, SLOT(setOn(unsigned char)));
            connect(RGBzones[i], SIGNAL(doOff(unsigned char)), this, SLOT(setOff(unsigned char)));
            connect(RGBzones[i], SIGNAL(doWhite(unsigned char)), this, SLOT(setWhite(unsigned char)));
            connect(RGBzones[i], SIGNAL(doBuildinEffect(unsigned char)), this, SLOT(setBuildinEffect(unsigned char)));
            connect(RGBzones[i], SIGNAL(doIncreaseSpeed(unsigned char)), this, SLOT(setIncreaseSpeed(unsigned char)));
            connect(RGBzones[i], SIGNAL(doDecreaseSpeed(unsigned char)), this, SLOT(setDecreaseSpeed(unsigned char)));
        }

        /* For all except the master connect to the Master. */
        if(i != 0){
            connect(RGBzones[0], SIGNAL(doColor(QColor, unsigned char)), RGBzones[i], SLOT(updateColor(QColor)));
            connect(RGBzones[0], SIGNAL(doBright(unsigned char, unsigned char)), RGBzones[i], SLOT(updateBright(unsigned char)));

            connect(RGBzones[0], SIGNAL(doOn(unsigned char)), RGBzones[i], SLOT(updateOn()));
            connect(RGBzones[0], SIGNAL(doOff(unsigned char)), RGBzones[i], SLOT(updateOff()));
            connect(RGBzones[0], SIGNAL(doWhite(unsigned char)), RGBzones[i], SLOT(updateWhite()));

            connect(RGBzones[0], SIGNAL(fadeEnabled()), RGBzones[i], SLOT(disableFade()));
            connect(RGBzones[i], SIGNAL(fadeEnabled()), RGBzones[0], SLOT(disableFade()));
        }

        /* Add the zone to the Menu for show/hide. */
        viewControllerMenu->addAction(RGBzones[i]->viewControllerAction);

        /* Add the zone to the Layout */
        rgbLayout->addWidget(RGBzones[i]);
    }

    m_rgbWidget->setLayout(rgbLayout);

    viewControllerMenu->addSeparator();

    m_whiteWidget = new QWidget(this);
    m_whiteWidget->setContentsMargins(0, 0, 0, 0);
    m_viewWhiteAction = new QAction(tr("White"), m_whiteWidget);
    m_viewWhiteAction->setCheckable(true);
    m_viewWhiteAction->setChecked(true);
    connect(m_viewWhiteAction, SIGNAL(toggled(bool)), m_whiteWidget, SLOT(setVisible(bool)));
    viewControllerMenu->addAction(m_viewWhiteAction);
    QHBoxLayout *whiteLayout = new QHBoxLayout();

    /* Create 5 zone controllers 1 Master, 4 Zones. */
    for(int i = 0; i <= 4; i++){

        /* Create controller, set title to Master / Zone. */
        if(i == 0){
            WhiteZones[i] = new WhiteController(tr(LC_WHITE_MASTER_NAME), i, this);
        }
        else{
            WhiteZones[i] = new WhiteController(tr(LC_WHITE_ZONE_NAME) + tr(" ") + QString::number(i), i, this);
        }

        viewControllerMenu->addAction(WhiteZones[i]->viewControllerAction);

        whiteLayout->addWidget(WhiteZones[i]);
    }

    m_whiteWidget->setLayout(whiteLayout);

    m_mainLayout->addWidget(m_rgbWidget);
    m_mainLayout->addWidget(m_whiteWidget);

    /* Set the layout to the main Widget. */
    m_mainWidget->setLayout(m_mainLayout);

    setName(m_name);

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

PresetLC *LightController::getPreset()
{
    PresetLC *plc = new PresetLC(m_id, this);
    for(int i = 0; i < 5; i++){
      plc->RGBzones[i] = RGBzones[i]->getPreset();
    }
    return plc;
}

void LightController::setPreset(PresetLC *p)
{
    bool setZone0 = true;

    if(!p->RGBzones[0]->m_fade){
        for(int i = 1; i < 5; i++){
            if(p->RGBzones[0]->m_enabled    != p->RGBzones[i]->m_enabled    ||
               p->RGBzones[0]->m_color      != p->RGBzones[i]->m_color      ||
               p->RGBzones[0]->m_brightness != p->RGBzones[i]->m_brightness ||
               p->RGBzones[1]->m_fixed      != p->RGBzones[i]->m_fixed      ||
               p->RGBzones[1]->m_fade       != p->RGBzones[i]->m_fade       ){
                setZone0 = false;
            }
        }
    }

    if(setZone0){
        RGBzones[0]->setPreset(p->RGBzones[0], true);
    }
    else{
        for(int i = 1; i < 5; i++){
            RGBzones[i]->setPreset(p->RGBzones[i], true);
        }
    }

}

void LightController::loadSettings(QSettings *s)
{
    setName(s->value(tr("name"), tr("Controller ") + QString::number(m_num)).toString());

    s->beginReadArray(tr("Zones"));
    for(int i = 0; i < 5; i++){
        s->setArrayIndex(i);
        RGBzones[i]->loadSettings(s);
    }
    s->endArray();
}

void LightController::saveSettings(QSettings *s)
{
    s->setValue(tr("id"), m_id);
    s->setValue(tr("name"), m_name);
    s->beginWriteArray(tr("Zones"));
    for(int i = 0; i < 5; i++){
        s->setArrayIndex(i);
        RGBzones[i]->saveSettings(s);
    }
    s->endArray();
}

bool LightController::areSomeFixed()
{
    return (RGBzones[1]->fixed() || RGBzones[2]->fixed() || RGBzones[3]->fixed() || RGBzones[4]->fixed());
}

void LightController::setName()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Set name"), tr("New name:"), QLineEdit::Normal, m_name, &ok);
    if(ok){
        setName(name);
    }
}

void LightController::setName(QString name)
{
    m_name = name;
    QString displayName = tr("[") + tr(LC_PREFIX) + QString::number(m_num) + tr("] ") + m_name;
    this->setWindowTitle(displayName);
    this->toggleViewAction()->setText(displayName);
    viewControllerMenu->setTitle(displayName);

    GLOBAL_settingsChanged = true;
}

void LightController::setBright(unsigned char value, unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->setBright(value, i);
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
            if(!RGBzones[i]->fixed()) m_udp->setColor(c, i);
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
            if(!RGBzones[i]->fixed()) m_udp->setOn(i);
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
            if(!RGBzones[i]->fixed()) m_udp->setOff(i);
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
            if(!RGBzones[i]->fixed()) m_udp->setWhite(i);
        }
    }
    else{
        m_udp->setWhite(zone);
    }
}

void LightController::setBuildinEffect(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->setBuildinEffect(i);
        }
    }
    else{
        m_udp->setBuildinEffect(zone);
    }
}

void LightController::setIncreaseSpeed(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->increaseSpeed(i);
        }
    }
    else{
        m_udp->increaseSpeed(zone);
    }
}

void LightController::setDecreaseSpeed(unsigned char zone)
{
    if(zone == 0 && areSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->decreaseSpeed(i);
        }
    }
    else{
        m_udp->decreaseSpeed(zone);
    }
}
