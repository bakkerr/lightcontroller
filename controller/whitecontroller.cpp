#include "whitecontroller.h"

WhiteController::WhiteController(QString name, unsigned char z, QWidget *parent) :
    QWidget(parent)
{
    m_zone = z;
    m_fixed = false;
    m_state = true;

    /* Setup the layout. */
    setupLayout();

    /* Create Actions. */
    viewControllerAction = new QAction(m_name, this);
    viewControllerAction->setCheckable(true);
    viewControllerAction->setChecked(true);
    connect(viewControllerAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));

    setName(name);

    /* Set the layout for this Widget. */
    this->setLayout(m_mainLayout);
}

WhiteController::~WhiteController()
{

}

void WhiteController::setupLayout()
{
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    m_groupBox = new QGroupBox(m_name, this);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(true);

    m_groupBox->setMinimumWidth(175);
    m_groupBox->setMaximumWidth(175);
    m_groupBox->setMinimumHeight(110);
    m_groupBox->setMaximumHeight(110);
    connect(m_groupBox, SIGNAL(toggled(bool)), this, SLOT(setState(bool)));

    QVBoxLayout *l1 = new QVBoxLayout();

    QHBoxLayout *l2 = new QHBoxLayout();
    m_brightDownButton = new QPushButton(tr("-"));
    m_brightDownButton->setToolTip(tr("Decrease brightness"));
    m_brightFullButton = new QPushButton(tr("Full"));
    m_brightFullButton->setToolTip(tr("Full brightness"));
    m_brightUpButton = new QPushButton(tr("+"));
    m_brightUpButton->setToolTip(tr("Increase brightness"));
    l2->addWidget(m_brightDownButton);
    l2->addWidget(m_brightFullButton);
    l2->addWidget(m_brightUpButton);

    QHBoxLayout *l3 = new QHBoxLayout();

    m_warmthDownButton = new QPushButton(tr("-"));
    m_warmthDownButton->setToolTip(tr("Decrease warmth"));
    m_warmthUpButton = new QPushButton(tr("+"));
    m_warmthUpButton->setToolTip(tr("Increase warmth"));
    m_nightButton = new QPushButton("Night", this);
    m_nightButton->setToolTip(tr("Enable Nightmode"));
    //connect(m_nightButton, SIGNAL(clicked()), this, SLOT(setNight()));

    l3->addWidget(m_warmthDownButton);
    l3->addWidget(m_nightButton);
    l3->addWidget(m_warmthUpButton);

    QHBoxLayout *l4 = new QHBoxLayout();

    m_fixedBox = new QCheckBox(tr("Fixed"), this);
    m_fixedBox->setToolTip(tr("If checked, this zone will ignore all external input. Less stable..."));
    m_fixedBox->setChecked(false);
    //if(m_zone <= 0 || m_zone > 4) m_fixedBox->setEnabled(false);
    connect(m_fixedBox, SIGNAL(toggled(bool)), this, SLOT(setFixed(bool)));
    l4->addWidget(m_fixedBox);

    l1->addLayout(l2);
    l1->addLayout(l3);
    l1->addLayout(l4);

    m_groupBox->setLayout(l1);
    m_groupBox->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_groupBox, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    m_mainLayout->addWidget(m_groupBox, 0, Qt::AlignTop | Qt::AlignLeft);
}

void WhiteController::contextMenu(const QPoint &x)
{
    QPoint gp = m_groupBox->mapToGlobal(x);

    QMenu myMenu;

    myMenu.addAction(viewControllerAction);
    myMenu.addSeparator();
    QAction *setNameAction = myMenu.addAction(tr("Change name"));

    connect(setNameAction, SIGNAL(triggered()), this, SLOT(setName()));

    myMenu.exec(gp);
}

PresetZone * WhiteController::getPreset()
{
    // FIXME
    return new PresetZone();
}

void WhiteController::setPreset(PresetZone *p, bool set)
{
    m_fixedBox->setChecked(p->m_fixed);

    if(set && p->m_enabled){
        setState(true);
   }
    else{
        if(set) setState(false);
    }
}

void WhiteController::loadSettings(QSettings *s)
{
    setName(s->value(tr("name"), tr("Zone ") + QString::number(m_zone)).toString());
    viewControllerAction->setChecked(s->value(tr("visible"), tr("true")).toBool());
}

void WhiteController::saveSettings(QSettings *s)
{
    s->setValue(tr("name"), m_name);
    s->setValue(tr("visible"), isVisible());
}

void WhiteController::setName()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Set name"), tr("Name:"), QLineEdit::Normal, m_name, &ok);
    if(ok){
        setName(name);
    }
}

void WhiteController::setName(QString name)
{
    if(m_name == name) return;

    m_name = name;
    QString displayName;

    if(m_zone == 255){
        displayName = tr("[") + tr(LC_WHITE_ZONE_PREFIX) + tr("0] ") + m_name;
    }
    else{
        displayName = tr("[") + tr(LC_WHITE_ZONE_PREFIX) + QString::number(m_zone) + tr("] ") + m_name;
    }
    m_groupBox->setTitle(displayName);
    viewControllerAction->setText(displayName);

    GLOBAL_settingsChanged = true;

}

void WhiteController::setState(bool state)
{
    if(state){
        updateOn();
        emit doOn(m_zone);
    }
    else {
        updateOff();
        emit doOff(m_zone);
    }
}

void WhiteController::setOnExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setState(true);
}

void WhiteController::setOffExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setState(false);
}

void WhiteController::setWhiteExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateWhite();
    setWhite();
}

void WhiteController::setBrightExt(unsigned char value){
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    updateBright(value);
    setBright(value);
}

void WhiteController::updateBright(unsigned char value)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    //m_brightSlider->blockSignals(true);
    //m_brightSlider->setValue(value);
    //m_brightSlider->blockSignals(false);
}

void WhiteController::updateWhite()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);
}

void WhiteController::updateOn()
{
    if(m_fixed) return;

    m_groupBox->blockSignals(true);
    m_groupBox->setChecked(true);
    m_groupBox->blockSignals(false);
}

void WhiteController::updateOff()
{
    if(m_fixed) return;

    m_groupBox->blockSignals(true);
    m_groupBox->setChecked(false);
    m_groupBox->blockSignals(false);
}


