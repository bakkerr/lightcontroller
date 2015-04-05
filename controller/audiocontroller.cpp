#include "audiocontroller.h"

const int BufferSize = 1000;


audioController::audioController(QWidget *parent) :
    QDockWidget(tr("Audio Controller"), parent),
    x(AUDIO_GRAPH_DISPLAY_SAMPLES),
    y(AUDIO_GRAPH_DISPLAY_SAMPLES),
    z(AUDIO_GRAPH_DISPLAY_SAMPLES),
    q(AUDIO_GRAPH_DISPLAY_SAMPLES)
{
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    threshold = AUDIO_THRESHOLD_DEFAULT;
    samples = AUDIO_SAMPLES_DEFAULT;
    lastBeat = 0;
    //audiothread = NULL;

    //QHBoxLayout *l0 = new QHBoxLayout();

    groupbox = new QGroupBox(tr("Enable"));
    groupbox->setCheckable(true);
    groupbox->setChecked(false);

    QVBoxLayout *l1 = new QVBoxLayout();

    QGroupBox *inputDevice = new QGroupBox(tr("Audio Input"));
    QHBoxLayout *l001 = new QHBoxLayout();

    m_deviceBox = new QComboBox(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    m_deviceBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }

    connect(m_deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    l001->addWidget(m_deviceBox);
    inputDevice->setLayout(l001);

    l1->addWidget(inputDevice);

    QHBoxLayout *l4 = new QHBoxLayout();

    settingsBox = new QGroupBox(tr("Trigger Settings"));
    settingsBox->setMaximumWidth(250);
    QVBoxLayout *settingsLayout = new QVBoxLayout();
    QHBoxLayout *thresholdLayout = new QHBoxLayout();
    thresholdSlider = new QSlider(Qt::Horizontal);
    thresholdSlider->setMaximum(AUDIO_THRESHOLD_MAX);
    thresholdSlider->setMinimum(AUDIO_THRESHOLD_MIN);
    thresholdSlider->setValue(threshold);
    thresholdSlider->setMaximumWidth(150);
    thresholdLabel = new QLabel(tr("Threshold:"));
    thresholdLabel->setMaximumWidth(100);
    thresholdLayout->addWidget(thresholdLabel);
    thresholdLayout->addWidget(thresholdSlider);
    settingsLayout->addLayout(thresholdLayout);
    QHBoxLayout *sampleLayout = new QHBoxLayout();
    sampleSlider = new QSlider(Qt::Horizontal);
    sampleSlider->setMinimum(AUDIO_SAMPLES_MIN);
    sampleSlider->setMaximum(AUDIO_SAMPLES_MAX);
    sampleSlider->setValue(samples);
    sampleSlider->setMaximumWidth(150);
    sampleLabel = new QLabel(tr("Samples:"));
    sampleLabel->setMaximumWidth(100);
    sampleLayout->addWidget(sampleLabel);
    sampleLayout->addWidget(sampleSlider);
    settingsLayout->addLayout(sampleLayout);
    manualTrigger = new QPushButton(tr("Trigger Manual"));
    connect(manualTrigger, SIGNAL(clicked()), this, SLOT(triggerEffect()));
    settingsLayout->addWidget(manualTrigger);
    settingsBox->setLayout(settingsLayout);
    l1->addWidget(settingsBox);



    effectBox = new QGroupBox(tr("Trigger Effect"));
    QVBoxLayout *effectLayout = new QVBoxLayout();
    effect = new QButtonGroup(this);
    effect->setExclusive(true);
    noEffectButton = new QRadioButton(tr("None"));
    randomSameButton = new QRadioButton(tr("Same random"));
    randomSameButton->setChecked(true);
    randomAllButton = new QRadioButton(tr("Different random"));
    fade10Button = new QRadioButton(tr("Fade +10"));
    fade20Button = new QRadioButton(tr("Fade +20"));
    effect->addButton(noEffectButton, EFFECT_NO);
    effect->addButton(randomSameButton, EFFECT_RANDOM_SAME);
    effect->addButton(randomAllButton, EFFECT_RANDOM_ALL);
    effect->addButton(fade10Button, EFFECT_FADE10);
    effect->addButton(fade20Button, EFFECT_FADE20);
    effectLayout->addWidget(noEffectButton);
    effectLayout->addWidget(randomSameButton);
    effectLayout->addWidget(randomAllButton);
    effectLayout->addWidget(fade10Button);
    effectLayout->addWidget(fade20Button);
    effectBox->setLayout(effectLayout);
    l1->addWidget(effectBox);

    l4->addLayout(l1);

    plot = new QCustomPlot();
    plot->setMinimumWidth(200);
    plot->addGraph();
    plot->addGraph();
    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::red));
    plot->graph(2)->setPen(QPen(Qt::green));

    plot->setBackground(groupbox->palette().background());
    plot->yAxis->setRange(-128, 127);
    plot->xAxis->setRange(0, AUDIO_GRAPH_DISPLAY_SAMPLES / (double)AUDIO_INCOMING_SAMPLES_PER_SEC);
    plot->replot();

    l4->addWidget(plot);

    groupbox->setLayout(l4);

    //l0->addWidget(groupbox);

    connect(groupbox, SIGNAL(clicked(bool)), this, SLOT(stateChange(bool)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setThreshold(int)));
    connect(sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));

    connect(this, SIGNAL(beatDetected()), this, SLOT(triggerEffect()));

    //this->setLayout(l0);
    setWidget(groupbox);

}

void audioController::stateChange(bool s)
{
    if(s) {
        startAudio();
    }
    else{
        stopAudio();
    }
}

void audioController::setThreshold(int value)
{
    threshold = value;
}

void audioController::setSamples(int value)
{
    samples = value;
}

void audioController::startAudio()
{
    m_Beat = new libbeat::BeatController(this, 4096, 44100, 192);
    connect(m_Beat, SIGNAL(beatDrum()), this, SLOT(triggerEffect()));
}


void audioController::stopAudio()
{
    delete m_Beat;
}

void audioController::doReplot(qint64 len)
{
    if (len > AUDIO_GRAPH_UPDATE_SAMPLES){
        len = AUDIO_GRAPH_UPDATE_SAMPLES;
    }
    qint64 s = 0;//m_ioDevice->read(m_buffer->data(), len);

    /* This is not efficient... */
    for(int i = s - 1; i >= 0; i--){
        y.pop_back();
        quint8 v = 0;//(quint8)m_buffer->at(i);
        qDebug() << v << endl;
        y.push_front((double)v-128);
    }

    /* Calculate graph data */
    for(int i = 0; i < AUDIO_GRAPH_DISPLAY_SAMPLES; i++){

        /* Set x-axis data in seconds */
        x[i] = (double)(i) / AUDIO_INCOMING_SAMPLES_PER_SEC;

        /* Threshold line (constant) */
        z[i] = threshold;

        /*
         * Draw vertical lines on samples interval
         * FIXME: Convert to detected "beats"
         */
        if(i % samples == 0){
            q[i] = 127;
        }
        else {
            q[i] = -128;
        }

    }

    /* FIXME: When do we want to appove a new beat detect? */
    lastBeat += s;

    /*
     * Do "beat" detect
     * FIXME: should do this somehow more advanced (fft?)
     */
    int max = 0;

    /* See if the last detected "beat" is at least more than #samples ago */
    if(lastBeat >= samples){
        //qDebug() << "Ready for another beat" << endl;

        /* Check for values above threshold, keep last position */
        for(int i = samples - 1; i >= 0; i--){
            if(y[i] > threshold){
                max = y[i];
                lastBeat = i;
            }
        }

        if(max > threshold){
            //qDebug() << "Beat Detected!" << endl;
            emit beatDetected();
        }
    }
    /*else{
        qDebug() << "Skipping detection..." << endl;
    }*/

    plot->graph(0)->setData(x, y);
    plot->graph(1)->setData(x, z);
    plot->graph(2)->setData(x, q);

    plot->replot();

}

void audioController::triggerEffect()
{
    switch(effect->checkedId()){
        case EFFECT_RANDOM_ALL:
            emit setRandomAll();
            break;
        case EFFECT_RANDOM_SAME:
            emit setRandomSame();
            break;
        case EFFECT_FADE10:
            emit fade10();
            break;
        case EFFECT_FADE20:
            emit fade20();
            break;
        case EFFECT_NO:
        default:
            break;

    }
}
