#include "singlecontroller.h"

SingleController::SingleController(quint16 id, QString name, quint16 remote, QList<SingleController*> slaves, quint16 containerID, QWidget *parent) :
    QWidget(parent)
{
    m_id = id;
    m_zone = remote;
    m_container = containerID;
    m_fixed = false;
    m_state = true;
    m_slaves = slaves;
    m_name = name;

    /* Setup the layout. */
    setupLayout();

    /* Create Actions. */
    viewControllerAction = new QAction(m_name, this);
    viewControllerAction->setCheckable(true);
    viewControllerAction->setChecked(true);
    connect(viewControllerAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));

    /* Set the layout for this Widget. */
    QHBoxLayout *ml = new QHBoxLayout();
    ml->setContentsMargins(0, 0, 0, 0);
    ml->addWidget(m_groupBox);
    this->setLayout(ml);
}

SingleController::~SingleController()
{
    delete m_wheel;
}

void SingleController::setupLayout()
{
    m_groupBox = new QGroupBox(m_name, this);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(true);

    m_groupBox->setMinimumWidth(175);
    m_groupBox->setMaximumWidth(175);
    m_groupBox->setMinimumHeight(325);
    m_groupBox->setMaximumHeight(325);
    connect(m_groupBox, SIGNAL(toggled(bool)), this, SLOT(setState(bool)));

    QVBoxLayout *l1 = new QVBoxLayout();

    m_wheel = new ColorWheel(this);
    connect(m_wheel, SIGNAL(colorChange(QColor)), this, SLOT(setColor(QColor)));

    QHBoxLayout *l2 = new QHBoxLayout();
    m_brightLabel = new QLabel(tr("Brightness"), this);
    m_brightLabel->setToolTip(tr("Set the Brightness for this zone"));
    l2->addWidget(m_brightLabel);
    m_brightSlider = new QSlider(Qt::Horizontal, this);
    m_brightSlider->setToolTip(tr("Set the Brightness for this zone"));
    m_brightSlider->setMinimumWidth(80);
    m_brightSlider->setMaximumWidth(80);
    m_brightSlider->setMinimum(0);
    m_brightSlider->setMaximum(18);
    m_brightSlider->setValue(BRIGHT_VALUE_DEFAULT);
    l2->addWidget(m_brightSlider, 0, Qt::AlignRight);
    connect(m_brightSlider, SIGNAL(valueChanged(int)), this, SLOT(setBright(int)));

    QHBoxLayout *l3 = new QHBoxLayout();
    m_fadeBox = new QCheckBox(tr("Fade"), this);
    m_fadeBox->setChecked(false);
    m_fadeBox->setToolTip(tr("Enable/Disable Fading"));
    connect(m_fadeBox, SIGNAL(toggled(bool)), this, SLOT(toggleFade(bool)));
    l3->addWidget(m_fadeBox);
    m_fadeSlider = new QSlider(Qt::Horizontal, this);
    m_fadeSlider->setToolTip(tr("Set the fading time"));
    m_fadeSlider->setMinimumWidth(80);
    m_fadeSlider->setMaximumWidth(80);
    m_fadeSlider->setMinimum(FADE_VALUE_MS_MIN);
    m_fadeSlider->setMaximum(FADE_VALUE_MS_MAX);
    m_fadeSlider->setValue(FADE_VALUE_MS_DEFAULT);
    connect(m_fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeTime(int)));
    l3->addWidget(m_fadeSlider, 0, Qt::AlignRight);
    m_fadeTimer = new QTimer(this);
    m_fadeTimer->stop();
    m_fadeTimer->setInterval(FADE_VALUE_MS_DEFAULT);
    connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(fade()));

    QHBoxLayout *l4 = new QHBoxLayout();

    m_fixedBox = new QCheckBox(tr("Fixed"), this);
    m_fixedBox->setToolTip(tr("If checked, this zone will ignore all external input. Less stable..."));
    m_fixedBox->setChecked(false);
    connect(m_fixedBox, SIGNAL(toggled(bool)), this, SLOT(setFixed(bool)));
    l4->addWidget(m_fixedBox);

    m_decSpeedButton = new QPushButton(tr("-"), this);
    m_decSpeedButton->setMaximumWidth(22);
    m_decSpeedButton->setToolTip(tr("Decrease buildin effect speed. Provided because it is a feature of the bulbs, it is not persistent, state cannot be updated..."));
    connect(m_decSpeedButton, SIGNAL(clicked()), this, SLOT(decreaseSpeed()));
    l4->addWidget(m_decSpeedButton);
    m_buildinEffectButton = new QPushButton(tr("M"), this);
    m_buildinEffectButton->setMaximumWidth(28);
    m_buildinEffectButton->setToolTip(tr("Set next buildin effect. Provided because it is a feature of the bulbs, it is not persistent, state cannot be updated..."));
    connect(m_buildinEffectButton, SIGNAL(clicked()), this, SLOT(setBuildinEffect()));
    l4->addWidget(m_buildinEffectButton);
    m_incSpeedButton = new QPushButton(tr("+"), this);
    m_incSpeedButton->setMaximumWidth(22);
    m_incSpeedButton->setToolTip(tr("Increase buildin effect speed. Provided because it is a feature of the bulbs, it is not persistent, state cannot be updated..."));
    connect(m_incSpeedButton, SIGNAL(clicked()), this, SLOT(increaseSpeed()));
    l4->addWidget(m_incSpeedButton);


    QHBoxLayout *l5 = new QHBoxLayout();

    m_randomButton = new QPushButton("Random", this);
    m_randomButton->setToolTip(tr("Set a random color"));
    connect(m_randomButton, SIGNAL(clicked()), this, SLOT(setRandom()));
    l5->addWidget(m_randomButton);

    m_whiteButton = new QPushButton("White", this);
    m_whiteButton->setToolTip(tr("White"));
    m_whiteButton->setStyleSheet("QPushButton {background-color: #FFFFFF}");
    connect(m_whiteButton, SIGNAL(clicked()), this, SLOT(setWhite()));
    l5->addWidget(m_whiteButton);

    QHBoxLayout *l6 = new QHBoxLayout();

    m_redButton = new QPushButton(tr("R"), this);
    m_redButton->setToolTip(tr("Red"));
    m_redButton->setStyleSheet("QPushButton {background-color: #FF0000}");
    connect(m_redButton, SIGNAL(clicked()), this, SLOT(red()));
    l6->addWidget(m_redButton);

    m_greenButton = new QPushButton(tr("G"), this);
    m_greenButton->setToolTip(tr("Green"));
    m_greenButton->setStyleSheet("QPushButton {background-color: #00FF00}");
    connect(m_greenButton, SIGNAL(clicked()), this, SLOT(green()));
    l6->addWidget(m_greenButton);

    m_blueButton = new QPushButton(tr("B"), this);
    m_blueButton->setToolTip(tr("Blue"));
    m_blueButton->setStyleSheet("QPushButton {background-color: #0000FF}");
    connect(m_blueButton, SIGNAL(clicked()), this, SLOT(blue()));
    l6->addWidget(m_blueButton);

    l1->addWidget(m_wheel);
    l1->addLayout(l2);
    l1->addLayout(l3);
    l1->addLayout(l4);
    l1->addLayout(l5);
    l1->addLayout(l6);

    m_groupBox->setLayout(l1);
    m_groupBox->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_groupBox, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

}

void SingleController::contextMenu(const QPoint &x)
{
    QPoint gp = m_groupBox->mapToGlobal(x);

    QMenu myMenu;

    myMenu.addAction(viewControllerAction);
    myMenu.addSeparator();

    QAction *setNameAction = myMenu.addAction(tr("Change name"));
    connect(setNameAction, SIGNAL(triggered()), this, SLOT(setName()));

    myMenu.addSeparator();

    QAction *sendPairAction = myMenu.addAction(tr("Send pair command"));
    connect(sendPairAction, SIGNAL(triggered()), this, SLOT(pair()));

    QAction *sendUnPairAction = myMenu.addAction(tr("Send unpair command"));
    connect(sendUnPairAction, SIGNAL(triggered()), this, SLOT(unPair()));

    myMenu.exec(gp);
}

PresetLC *SingleController::getPreset()
{
    return new PresetLC(m_id, m_groupBox->isChecked(), m_wheel->color(), m_fadeBox->isChecked(), m_fadeSlider->value(), m_fixed, m_brightSlider->value(), this);
}

void SingleController::setPreset(PresetLC *p, bool set)
{

    m_fixedBox->setChecked(p->m_fixed);

    m_fadeSlider->setValue(p->m_fadeTime);


    if(set && p->m_enabled){
        setState(true);
        if(p->m_color == Qt::white){
            setWhiteExt();
        }
        else{
            this->setColorExt(p->m_color);
        }
        setBrightExt(p->m_brightness);
        m_fadeBox->setChecked(p->m_fade);
    }
    else{
        m_brightSlider->blockSignals(true);
        m_brightSlider->setValue(p->m_brightness);
        m_brightSlider->blockSignals(false);
        m_fadeBox->blockSignals(true);
        m_fadeBox->setChecked(p->m_fade);
        m_fadeBox->blockSignals(false);
        m_wheel->setColor(p->m_color);
        if(set) setState(false);
    }
}

void SingleController::loadSettings(QSettings *s)
{
    setName(s->value(tr("name"), tr("Zone ") + QString::number(m_zone)).toString());
}

void SingleController::saveSettings(QSettings *s)
{
    s->setValue(tr("name"), m_name);
}

void SingleController::setName()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Set name"), tr("Name:"), QLineEdit::Normal, m_name, &ok);
    if(ok){
        setName(name);
    }
}

void SingleController::setName(QString name)
{
    if(m_name == name) return;

    m_name = name;

    viewControllerAction->setText(m_name);
    m_groupBox->setTitle(m_name);

    GLOBAL_settingsChanged = true;
}

void SingleController::setState(bool state)
{
    if(m_zone == 0 || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            state ? slave->setOnExt() : slave->setOffExt();
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            state ? slave->updateOn() : slave->updateOff();
        }
        state ? emit doOn(m_zone) : emit doOff(m_zone);
    }

    state ? updateOn() : updateOff();

}

void SingleController::toggleFade(bool state){
    if(state){
        m_fadeTimer->start();
        emit fadeEnabled();
    }
    else{
        m_fadeTimer->stop();
        emit fadeDisabled();
    }
}

void SingleController::enableFade()
{
    if(!m_fixed){
        m_fadeBox->blockSignals(true);
        m_fadeTimer->stop();
        m_fadeBox->setChecked(true);
        m_fadeBox->setEnabled(false);
    }
}

void SingleController::disableFade()
{
    if(!m_fixed){
        m_fadeBox->setChecked(false);
        m_fadeBox->setEnabled(true);
        m_fadeTimer->stop();
        m_fadeBox->blockSignals(false);
    }
}

void SingleController::setFadeTime(int msec)
{
    m_fadeTimer->setInterval(msec);
}

void SingleController::fade(int n)
{
    QColor c = m_wheel->color();
    c.setHsv((c.hue() + n) % 360, 0, 0);
    m_wheel->changeColor(c);
}

bool SingleController::areSlavesFixed(){
    foreach(SingleController* slave, m_slaves){
        if(slave->fixed()){
            return true;
        }
    }
    return false;
}

void SingleController::setColor(const QColor &color)
{
    if(m_slaves.empty()){
      emit doColor(color, m_zone);
    }
    else if(m_zone == 0 || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            slave->setColorExt(color);
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            slave->updateColor(color);
        }
        emit doColor(color, m_zone);
    }
}

void SingleController::setBright(int value)
{
    if(m_slaves.empty()){
        emit doBright((quint8)value, m_zone);
    }
    else if(m_zone == 0 || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            slave->setBrightExt(value);
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            slave->updateBright(value);
        }
        emit doBright((quint8)value, m_zone);
    }
}

void SingleController::setWhite()
{
    if(m_slaves.empty()){
      emit doWhite(m_zone);
    }
    else if(m_zone == 0 || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            slave->setWhiteExt();
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            slave->updateWhite();
        }
        emit doWhite(m_zone);
    }

    m_wheel->setInnerColor(Qt::white);
}

void SingleController::setNight()
{
    if(m_slaves.empty()){
      emit doNight(m_zone);
    }
    else if(m_zone == 0 || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            slave->setNightExt();
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            slave->updateNight();
        }
        emit doNight(m_zone);
    }

    m_wheel->setInnerColor(Qt::white);
}

void SingleController::setBuildinEffect()
{
    //emit doBuildinEffect(m_zone);
}

void SingleController::decreaseSpeed()
{
    //emit doDecreaseSpeed(m_zone);
}

void SingleController::increaseSpeed()
{
    //emit doIncreaseSpeed(m_zone);
}

void SingleController::setRandom(bool same)
{
    QColor c;
    c.setHsv(rand() % 360, 0, 0);

    if(m_slaves.empty()){
      setColorExt(c);
    }
    else if(m_zone == 0 || !same || areSlavesFixed()){
        foreach(SingleController* slave, m_slaves){
            if(same){
                slave->setColorExt(c);
            }
            else{
                slave->setRandomExtDifferent();
            }
            updateColor(c);
        }
    }
    else{
        foreach(SingleController* slave, m_slaves){
            slave->updateColor(c);
        }
        setColorExt(c);
    }
}

void SingleController::flash(){
    QColor c = color();
    if(c.hue() != QColor(Qt::white).hue()){
        setWhiteExt();
        setColorExt(c);
    }
    else{
        setRandomExt();
        setWhiteExt();
    }
}

void SingleController::setOnExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setState(true);
}

void SingleController::setOffExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setState(false);
}

void SingleController::setColorExt(const QColor &color)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateColor(color);
    setColor(color);
}

void SingleController::setBrightExt(unsigned char value){
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateBright(value);
    setBright(value);
}

void SingleController::setWhiteExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateWhite();
    setWhite();
}

void SingleController::setNightExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateNight();
    setNight();
}

void SingleController::setRandomExt(bool same)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setRandom(same);
}

void SingleController::updateOn()
{
    if(m_fixed) return;

    m_groupBox->blockSignals(true);
    m_groupBox->setChecked(true);
    m_wheel->setInnerColor(m_wheel->color());
    m_groupBox->blockSignals(false);

}

void SingleController::updateOff()
{
    if(m_fixed) return;

    m_groupBox->blockSignals(true);
    m_groupBox->setChecked(false);
    m_wheel->setInnerColor(Qt::gray);
    m_groupBox->blockSignals(false);

}

void SingleController::updateColor(const QColor &color)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    m_wheel->setColor(color);
}

void SingleController::updateBright(unsigned char value)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    m_brightSlider->blockSignals(true);
    m_brightSlider->setValue(value);
    m_brightSlider->blockSignals(false);
}

void SingleController::updateWhite()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    m_wheel->setInnerColor(Qt::white);
}

void SingleController::updateNight()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    m_wheel->setInnerColor(Qt::white);
}
