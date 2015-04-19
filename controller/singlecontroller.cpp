#include "singlecontroller.h"

SingleController::SingleController(QString name, unsigned char z, QWidget *parent) :
    QWidget(parent)
{
    this->zone = z;
    m_fixed = false;

    QHBoxLayout *mainLayout = new QHBoxLayout;

    groupbox = new QGroupBox(name);
    groupbox->setCheckable(true);
    groupbox->setChecked(true);

    groupbox->setMinimumWidth(175);
    groupbox->setMaximumWidth(175);
    groupbox->setMinimumHeight(325);
    groupbox->setMaximumHeight(325);
    connect(groupbox, SIGNAL(toggled(bool)), this, SLOT(changeState(bool)));

    QVBoxLayout *l1 = new QVBoxLayout();

    wheel = new ColorWheel();
    connect(wheel, SIGNAL(colorChange(QColor)), this, SLOT(color(QColor)));

    fixedBox = new QCheckBox(tr("Fixed"));
    fixedBox->setChecked(false);
    connect(fixedBox, SIGNAL(toggled(bool)), this, SLOT(setFixed(bool)));

    QHBoxLayout *l2 = new QHBoxLayout();
    brightBox = new QLabel(tr("Brightness"));
    l2->addWidget(brightBox);
    brightSlider = new QSlider(Qt::Horizontal);
    brightSlider->setMinimumWidth(80);
    brightSlider->setMaximumWidth(80);
    brightSlider->setMinimum(1);
    brightSlider->setMaximum(19);
    brightSlider->setValue(BRIGHT_VALUE_DEFAULT);
    l2->addWidget(brightSlider, 0, Qt::AlignRight);
    connect(brightSlider, SIGNAL(valueChanged(int)), this, SLOT(bright(int)));

    QHBoxLayout *l3 = new QHBoxLayout();
    fadeBox = new QCheckBox(tr("Fade"));
    fadeBox->setChecked(false);
    connect(fadeBox, SIGNAL(toggled(bool)), this, SLOT(toggleFade(bool)));
    l3->addWidget(fadeBox);
    fadeSlider = new QSlider(Qt::Horizontal);
    fadeSlider->setMinimumWidth(80);
    fadeSlider->setMaximumWidth(80);
    fadeSlider->setMinimum(FADE_VALUE_MS_MIN);
    fadeSlider->setMaximum(FADE_VALUE_MS_MAX);
    fadeSlider->setValue(FADE_VALUE_MS_DEFAULT);
    connect(fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeTime(int)));
    l3->addWidget(fadeSlider, 0, Qt::AlignRight);
    timer = new QTimer();
    timer->stop();
    timer->setInterval(FADE_VALUE_MS_DEFAULT);
    connect(timer, SIGNAL(timeout()), this, SLOT(fade()));

    QHBoxLayout *l4 = new QHBoxLayout();

    randomButton = new QPushButton("Random");
    connect(randomButton, SIGNAL(clicked()), this, SLOT(setRandom()));
    l4->addWidget(randomButton);

    whiteButton = new QPushButton("White");
    whiteButton->setStyleSheet("QPushButton {background-color: #FFFFFF}");
    connect(whiteButton, SIGNAL(clicked()), this, SLOT(white()));
    l4->addWidget(whiteButton);

    QHBoxLayout *l5 = new QHBoxLayout();

    redButton = new QPushButton(tr("R"));
    redButton->setStyleSheet("QPushButton {background-color: #FF0000}");
    connect(redButton, SIGNAL(clicked()), this, SLOT(red()));
    l5->addWidget(redButton);

    greenButton = new QPushButton(tr("G"));
    greenButton->setStyleSheet("QPushButton {background-color: #00FF00}");
    connect(greenButton, SIGNAL(clicked()), this, SLOT(green()));
    l5->addWidget(greenButton);

    blueButton = new QPushButton(tr("B"));
    blueButton->setStyleSheet("QPushButton {background-color: #0000FF}");
    connect(blueButton, SIGNAL(clicked()), this, SLOT(blue()));
    l5->addWidget(blueButton);

    l1->addWidget(wheel);
    l1->addLayout(l2);
    l1->addLayout(l3);
    l1->addWidget(fixedBox);
    l1->addLayout(l4);
    l1->addLayout(l5);

    groupbox->setLayout(l1);

    mainLayout->addWidget(groupbox, 0, Qt::AlignTop | Qt::AlignLeft);

    viewControllerAction = new QAction(name, this);
    viewControllerAction->setCheckable(true);
    viewControllerAction->setChecked(true);
    connect(viewControllerAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));

    this->setLayout(mainLayout);
}

SingleController::~SingleController()
{
    delete wheel;
}

void SingleController::changeState(bool state)
{
    if(state){
        emit doOn(zone);
    }
    else {
        emit doOff(zone);
    }
}

void SingleController::changeColor(const QColor &color)
{
    wheel->changeColor(color);
}

void SingleController::setColor(const QColor &color)
{
    if(m_fixed) return;

    wheel->setColor(color);
}

void SingleController::setBright(unsigned char value)
{
    if(m_fixed) return;

    brightSlider->blockSignals(true);
    brightSlider->setValue(value);
    brightSlider->blockSignals(false);
}

void SingleController::setWhite()
{
    if(m_fixed) return;

    wheel->setColor(Qt::white);
}

void SingleController::setRandom()
{
    if(m_fixed) return;

    QColor c;
    c.setHsv(rand() % 360, 0, 0);
    wheel->changeColor(c);
}

void SingleController::toggleFade(bool state){
    if(state){
        timer->start();
        emit fadeEnabled();
    }
    else{
        timer->stop();
    }
}

void SingleController::fade(int n)
{
    QColor c = wheel->color();
    c.setHsv((c.hue() + n) % 360, 0, 0);
    wheel->changeColor(c);
}

void SingleController::setOn()
{
    wheel->setColor(Qt::white);
}

void SingleController::setOff()
{
    wheel->setColor(Qt::gray);
}


