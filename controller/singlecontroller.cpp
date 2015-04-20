#include "singlecontroller.h"

SingleController::SingleController(QString name, unsigned char z, QWidget *parent) :
    QWidget(parent)
{
    /* Set local state variables. */
    m_name = name;
    m_zone = z;
    m_fixed = false;

    /* Setup the layout. */
    setupLayout();

    /* Create Actions. */
    viewControllerAction = new QAction(name, this);
    viewControllerAction->setCheckable(true);
    viewControllerAction->setChecked(true);
    connect(viewControllerAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));

    /* Set the layout for this Widget. */
    this->setLayout(m_mainLayout);
}

SingleController::~SingleController()
{
    delete m_wheel;
}

void SingleController::setupLayout()
{
    m_mainLayout = new QHBoxLayout;

    m_groupBox = new QGroupBox(m_name);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(true);

    m_groupBox->setMinimumWidth(175);
    m_groupBox->setMaximumWidth(175);
    m_groupBox->setMinimumHeight(325);
    m_groupBox->setMaximumHeight(325);
    connect(m_groupBox, SIGNAL(toggled(bool)), this, SLOT(setState(bool)));

    QVBoxLayout *l1 = new QVBoxLayout();

    m_wheel = new ColorWheel();
    connect(m_wheel, SIGNAL(colorChange(QColor)), this, SLOT(color(QColor)));

    m_fixedBox = new QCheckBox(tr("Fixed"));
    m_fixedBox->setToolTip(tr("If checked, this zone will ignore all external input."));
    m_fixedBox->setChecked(false);
    if(m_zone <= 0 || m_zone > 4) m_fixedBox->setEnabled(false);
    connect(m_fixedBox, SIGNAL(toggled(bool)), this, SLOT(setFixed(bool)));

    QHBoxLayout *l2 = new QHBoxLayout();
    m_brightLabel = new QLabel(tr("Brightness"));
    l2->addWidget(m_brightLabel);
    m_brightSlider = new QSlider(Qt::Horizontal);
    m_brightSlider->setToolTip(tr("Set the Brightness of this zone"));
    m_brightSlider->setMinimumWidth(80);
    m_brightSlider->setMaximumWidth(80);
    m_brightSlider->setMinimum(1);
    m_brightSlider->setMaximum(19);
    m_brightSlider->setValue(BRIGHT_VALUE_DEFAULT);
    l2->addWidget(m_brightSlider, 0, Qt::AlignRight);
    connect(m_brightSlider, SIGNAL(valueChanged(int)), this, SLOT(bright(int)));

    QHBoxLayout *l3 = new QHBoxLayout();
    m_fadeBox = new QCheckBox(tr("Fade"));
    m_fadeBox->setChecked(false);
    connect(m_fadeBox, SIGNAL(toggled(bool)), this, SLOT(toggleFade(bool)));
    l3->addWidget(m_fadeBox);
    m_fadeSlider = new QSlider(Qt::Horizontal);
    m_fadeSlider->setToolTip(tr("Set the fading time"));
    m_fadeSlider->setMinimumWidth(80);
    m_fadeSlider->setMaximumWidth(80);
    m_fadeSlider->setMinimum(FADE_VALUE_MS_MIN);
    m_fadeSlider->setMaximum(FADE_VALUE_MS_MAX);
    m_fadeSlider->setValue(FADE_VALUE_MS_DEFAULT);
    connect(m_fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeTime(int)));
    l3->addWidget(m_fadeSlider, 0, Qt::AlignRight);
    m_fadeTimer = new QTimer();
    m_fadeTimer->stop();
    m_fadeTimer->setInterval(FADE_VALUE_MS_DEFAULT);
    connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(fade()));

    QHBoxLayout *l4 = new QHBoxLayout();

    m_randomButton = new QPushButton("Random");
    m_randomButton->setToolTip(tr("Set a random color"));
    connect(m_randomButton, SIGNAL(clicked()), this, SLOT(setRandom()));
    l4->addWidget(m_randomButton);

    m_whiteButton = new QPushButton("White");
    m_whiteButton->setToolTip(tr("White"));
    m_whiteButton->setStyleSheet("QPushButton {background-color: #FFFFFF}");
    connect(m_whiteButton, SIGNAL(clicked()), this, SLOT(white()));
    l4->addWidget(m_whiteButton);

    QHBoxLayout *l5 = new QHBoxLayout();

    m_redButton = new QPushButton(tr("R"));
    m_redButton->setToolTip(tr("Red"));
    m_redButton->setStyleSheet("QPushButton {background-color: #FF0000}");
    connect(m_redButton, SIGNAL(clicked()), this, SLOT(red()));
    l5->addWidget(m_redButton);

    m_greenButton = new QPushButton(tr("G"));
    m_greenButton->setToolTip(tr("Green"));
    m_greenButton->setStyleSheet("QPushButton {background-color: #00FF00}");
    connect(m_greenButton, SIGNAL(clicked()), this, SLOT(green()));
    l5->addWidget(m_greenButton);

    m_blueButton = new QPushButton(tr("B"));
    m_blueButton->setToolTip(tr("Blue"));
    m_blueButton->setStyleSheet("QPushButton {background-color: #0000FF}");
    connect(m_blueButton, SIGNAL(clicked()), this, SLOT(blue()));
    l5->addWidget(m_blueButton);

    l1->addWidget(m_wheel);
    l1->addLayout(l2);
    l1->addLayout(l3);
    l1->addWidget(m_fixedBox);
    l1->addLayout(l4);
    l1->addLayout(l5);

    m_groupBox->setLayout(l1);

    m_mainLayout->addWidget(m_groupBox, 0, Qt::AlignTop | Qt::AlignLeft);
}

void SingleController::setState(bool state)
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

void SingleController::changeColor(const QColor &color)
{
    m_wheel->changeColor(color);
}

void SingleController::toggleFade(bool state){
    if(state){
        m_fadeTimer->start();
        emit fadeEnabled();
    }
    else{
        m_fadeTimer->stop();
    }
}

void SingleController::fade(int n)
{
    QColor c = m_wheel->color();
    c.setHsv((c.hue() + n) % 360, 0, 0);
    m_wheel->changeColor(c);
}

void SingleController::setRandom()
{
    QColor c;
    c.setHsv(rand() % 360, 0, 0);
    m_wheel->changeColor(c);
}

void SingleController::setRandomExt()
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    setRandom();
}

void SingleController::setColorExt(const QColor &color)
{
    if(m_fixed) return;

    m_groupBox->setChecked(true);

    m_wheel->setColor(color);
}

void SingleController::setBrightExt(unsigned char value)
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
