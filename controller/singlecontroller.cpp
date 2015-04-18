#include "singlecontroller.h"

SingleController::SingleController(QString name, unsigned char z, QWidget *parent) :
    QWidget(parent)
{
    this->zone = z;

    QHBoxLayout *mainLayout = new QHBoxLayout;

    groupbox = new QGroupBox(name);
    groupbox->setCheckable(true);
    groupbox->setChecked(true);

    groupbox->setMinimumWidth(175);
    groupbox->setMaximumWidth(175);
    groupbox->setMinimumHeight(375);
    groupbox->setMaximumHeight(375);
    connect(groupbox, SIGNAL(toggled(bool)), this, SLOT(changeState(bool)));

    QVBoxLayout *l1 = new QVBoxLayout();

    wheel = new ColorWheel();
    connect(wheel, SIGNAL(colorChange(QColor)), this, SLOT(color(QColor)));

    QHBoxLayout *l2 = new QHBoxLayout();
    brightBox = new QGroupBox(tr("Brightness"));
    brightSlider = new QSlider(Qt::Horizontal);
    brightSlider->setMinimum(1);
    brightSlider->setMaximum(19);
    brightSlider->setValue(BRIGHT_VALUE_DEFAULT);
    l2->addWidget(brightSlider);
    brightBox->setLayout(l2);
    connect(brightSlider, SIGNAL(valueChanged(int)), this, SLOT(bright(int)));


    QHBoxLayout *l3 = new QHBoxLayout();
    fadeBox = new QGroupBox(tr("Fade"));
    fadeBox->setCheckable(true);
    fadeBox->setChecked(false);
    connect(fadeBox, SIGNAL(toggled(bool)), this, SLOT(toggleFade(bool)));
    fadeSlider = new QSlider(Qt::Horizontal);
    //fadeSlider->setMaximumWidth(140);
    fadeSlider->setMinimum(FADE_VALUE_MS_MIN);
    fadeSlider->setMaximum(FADE_VALUE_MS_MAX);
    fadeSlider->setValue(FADE_VALUE_MS_DEFAULT);
    connect(fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeTime(int)));
    l3->addWidget(fadeSlider);
    fadeBox->setLayout(l3);
    timer = new QTimer();
    timer->stop();
    timer->setInterval(FADE_VALUE_MS_DEFAULT);
    connect(timer, SIGNAL(timeout()), this, SLOT(fade()));

    QVBoxLayout *l4 = new QVBoxLayout();


    QHBoxLayout *l5 = new QHBoxLayout();

    randomButton = new QPushButton("Random");
    connect(randomButton, SIGNAL(clicked()), this, SLOT(setRandom()));
    l5->addWidget(randomButton);

    whiteButton = new QPushButton("White");
    whiteButton->setStyleSheet("QPushButton {background-color: #FFFFFF}");
    connect(whiteButton, SIGNAL(clicked()), this, SLOT(white()));
    l5->addWidget(whiteButton);

    l4->addLayout(l5);

    QHBoxLayout *l6 = new QHBoxLayout();
    buttonBox = new QGroupBox(tr("Color"));

    redButton = new QPushButton(tr("R"));
    redButton->setStyleSheet("QPushButton {background-color: #FF0000}");
    connect(redButton, SIGNAL(clicked()), this, SLOT(red()));
    l6->addWidget(redButton);

    greenButton = new QPushButton(tr("G"));
    greenButton->setStyleSheet("QPushButton {background-color: #00FF00}");
    connect(greenButton, SIGNAL(clicked()), this, SLOT(green()));
    l6->addWidget(greenButton);

    blueButton = new QPushButton(tr("B"));
    blueButton->setStyleSheet("QPushButton {background-color: #0000FF}");
    connect(blueButton, SIGNAL(clicked()), this, SLOT(blue()));
    l6->addWidget(blueButton);

    l4->addLayout(l6);

    buttonBox->setLayout(l4);

    l1->addWidget(wheel);
    l1->addWidget(brightBox);
    l1->addWidget(fadeBox);
    l1->addWidget(buttonBox);

    groupbox->setLayout(l1);

    mainLayout->addWidget(groupbox);

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
    wheel->setColor(color);
}

void SingleController::setBright(unsigned char value)
{
    brightSlider->blockSignals(true);
    brightSlider->setValue(value);
    brightSlider->blockSignals(false);
}

void SingleController::setWhite()
{
    wheel->setColor(Qt::white);
}

void SingleController::setRandom()
{
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


