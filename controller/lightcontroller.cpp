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
    m_mainWidget->setContentsMargins(4, 4, 4, 4);
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
    m_mainLayout->setContentsMargins(0 ,0 ,0 ,0);

    m_rgbWidget = new QWidget(this);
    m_rgbWidget->setContentsMargins(0, 0, 0, 0);
    m_viewRGBAction = new QAction(tr("RGB"), m_rgbWidget);
    m_viewRGBAction->setCheckable(true);
    m_viewRGBAction->setChecked(true);
    connect(m_viewRGBAction, SIGNAL(toggled(bool)), m_rgbWidget, SLOT(setVisible(bool)));
    viewControllerMenu->addAction(m_viewRGBAction);
    QHBoxLayout *rgbLayout = new QHBoxLayout();
    rgbLayout->setContentsMargins(0, 0, 0, 0);

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
            connect(RGBzones[i], SIGNAL(doColor(QColor, unsigned char)), this, SLOT(RGBSetColor(QColor, unsigned char)));
            connect(RGBzones[i], SIGNAL(doBright(unsigned char, unsigned char)), this, SLOT(RGBSetBright(unsigned char, unsigned char)));
            connect(RGBzones[i], SIGNAL(doOn(unsigned char)), this, SLOT(RGBSetOn(unsigned char)));
            connect(RGBzones[i], SIGNAL(doOff(unsigned char)), this, SLOT(RGBSetOff(unsigned char)));
            connect(RGBzones[i], SIGNAL(doWhite(unsigned char)), this, SLOT(RGBSetWhite(unsigned char)));
            connect(RGBzones[i], SIGNAL(doBuildinEffect(unsigned char)), this, SLOT(RGBSetBuildinEffect(unsigned char)));
            connect(RGBzones[i], SIGNAL(doIncreaseSpeed(unsigned char)), this, SLOT(RGBIncreaseSpeed(unsigned char)));
            connect(RGBzones[i], SIGNAL(doDecreaseSpeed(unsigned char)), this, SLOT(RGBDecreaseSpeed(unsigned char)));
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
    whiteLayout->setContentsMargins(0 ,0 ,0 ,0);

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

    /*QToolBar *tb = new QToolBar();
    tb->setContentsMargins(0, 0, 0, 0);
    tb->addAction(m_viewRGBAction);
    for(int i = 0; i < 5; i++) tb->addAction(RGBzones[i]->viewControllerAction);
    tb->addSeparator();
    tb->addAction(m_viewWhiteAction);
    for(int i = 0; i < 5; i++) tb->addAction(WhiteZones[i]->viewControllerAction);
    tb->setMaximumHeight(20);
    m_mainLayout->addWidget(tb);*/

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

bool LightController::RGBAreSomeFixed()
{
    return (RGBzones[1]->fixed() || RGBzones[2]->fixed() || RGBzones[3]->fixed() || RGBzones[4]->fixed());
}

bool LightController::WhiteAreSomeFixed()
{
    return (WhiteZones[1]->fixed() || WhiteZones[2]->fixed() || WhiteZones[3]->fixed() || WhiteZones[4]->fixed());
}

void LightController::RGBSetBright(unsigned char value, unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetBright(value, i);
        }
    }
    else{
        m_udp->RGBSetBright(value, zone);
    }
}

void LightController::RGBSetColor(QColor c, unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetColor(c, i);
        }
    }
    else{
        m_udp->RGBSetColor(c, zone);
    }
}

void LightController::RGBSetOn(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetOn(i);
        }
    }
    else{
        m_udp->RGBSetOn(zone);
    }
}

void LightController::RGBSetOff(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetOff(i);
        }
    }
    else{
        m_udp->RGBSetOff(zone);
    }
}

void LightController::RGBSetWhite(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetWhite(i);
        }
    }
    else{
        m_udp->RGBSetWhite(zone);
    }
}

void LightController::RGBSetBuildinEffect(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBSetBuildinEffect(i);
        }
    }
    else{
        m_udp->RGBSetBuildinEffect(zone);
    }
}

void LightController::RGBIncreaseSpeed(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBIncreaseSpeed(i);
        }
    }
    else{
        m_udp->RGBIncreaseSpeed(zone);
    }
}

void LightController::RGBDecreaseSpeed(unsigned char zone)
{
    if(zone == 0 && RGBAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!RGBzones[i]->fixed()) m_udp->RGBDecreaseSpeed(i);
        }
    }
    else{
        m_udp->RGBDecreaseSpeed(zone);
    }
}


void LightController::WhiteSetOn(unsigned char zone)
{
    if(zone == 0 && WhiteAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!WhiteZones[i]->fixed()) m_udp->WhiteSetOn(i);
        }
    }
    else{
        m_udp->WhiteSetOn(zone);
    }
}

void LightController::WhiteSetOff(unsigned char zone)
{
    if(zone == 0 && WhiteAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!WhiteZones[i]->fixed()) m_udp->WhiteSetOff(i);
        }
    }
    else{
        m_udp->WhiteSetOff(zone);
    }
}

void LightController::WhiteSetNight(unsigned char zone)
{
    if(zone == 0 && WhiteAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!WhiteZones[i]->fixed()) m_udp->WhiteSetNight(i);
        }
    }
    else{
        m_udp->WhiteSetNight(zone);
    }
}

void LightController::WhiteIncreaseBright(unsigned char zone)
{
    if(zone == 0 && WhiteAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(WhiteZones[i]->fixed()) m_udp->WhiteIncreaseBright(i);
        }
    }
    else{
        m_udp->WhiteIncreaseBright(zone);
    }
}

void LightController::WhiteDecreaseBright(unsigned char zone)
{
    if(zone == 0 && WhiteAreSomeFixed()){
        for(int i = 1; i <= 4; i++){
            if(!WhiteZones[i]->fixed()) m_udp->WhiteDecreaseBright(i);
        }
    }
    else{
        m_udp->WhiteDecreaseBright(zone);
    }
}
