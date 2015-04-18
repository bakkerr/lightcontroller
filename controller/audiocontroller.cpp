#include "audiocontroller.h"

const int BufferSize = 1000;


audioController::audioController(QWidget *parent) :
    QDockWidget(tr("Audio Controller"), parent),
    x(AUDIO_GRAPH_DISPLAY_SAMPLES),
    y(AUDIO_GRAPH_DISPLAY_SAMPLES),
    z(AUDIO_GRAPH_DISPLAY_SAMPLES)
{
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    samples = AUDIO_SAMPLES_DEFAULT;
    lastBeat = 0;

    groupbox = new QGroupBox(tr("Enable"));
    groupbox->setCheckable(true);
    groupbox->setChecked(false);

    QVBoxLayout *l1 = new QVBoxLayout();

    QGroupBox *inputDevice = new QGroupBox(tr("Audio Input"));
    QHBoxLayout *l001 = new QHBoxLayout();

    m_deviceBox = new QComboBox(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    m_deviceBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    m_deviceBox->setMaximumWidth(230);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }
    m_deviceBox->setEnabled(false);

    connect(m_deviceBox, SIGNAL(activated(int)), SLOT(deviceChanged(int)));
    l001->addWidget(m_deviceBox);
    inputDevice->setLayout(l001);

    l1->addWidget(inputDevice);

    QHBoxLayout *l4 = new QHBoxLayout();

    settingsBox = new QGroupBox(tr("Trigger Settings"));
    settingsBox->setMaximumWidth(250);
    QVBoxLayout *settingsLayout = new QVBoxLayout();

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

    QHBoxLayout *fftWindowLayout = new QHBoxLayout();
    m_fftWindowBox = new QComboBox(this);
    m_fftWindowLabel = new QLabel(tr("FFT Window:"));
    m_fftWindowBox->addItem(tr("None"));
    m_fftWindowBox->addItem(tr("Hanning"));
    m_fftWindowBox->addItem(tr("Blackman"));
    connect(m_fftWindowBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFFTWindow(QString)));
    fftWindowLayout->addWidget(m_fftWindowLabel);
    fftWindowLayout->addWidget(m_fftWindowBox);
    settingsLayout->addLayout(fftWindowLayout);

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

    QVBoxLayout *l5 = new QVBoxLayout();

    plot = new QCustomPlot();
    plot->setMinimumWidth(200);
    plot->addGraph();
    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::green));

    for(int i = 0; i < AUDIO_GRAPH_DISPLAY_SAMPLES; i++){
        x[i] = (double)(i) / (AUDIO_INCOMING_SAMPLES_PER_SEC / AUDIO_AVERAGE_SAMPLES);
    }

    plot->setBackground(groupbox->palette().background());
    plot->yAxis->setRange(-30000, 30000);
    plot->yAxis->setTickLabels(false);
    plot->xAxis->setRange(0, AUDIO_GRAPH_DISPLAY_SAMPLES / ((double)AUDIO_INCOMING_SAMPLES_PER_SEC/AUDIO_AVERAGE_SAMPLES));

    plot->replot();

    l5->addWidget(plot);

    m_fft = new FFTDisplay(NULL);
    l5->addWidget(m_fft);

    l4->addLayout(l5);

    groupbox->setLayout(l4);    

    connect(groupbox, SIGNAL(clicked(bool)), this, SLOT(stateChange(bool)));
    connect(sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));

    connect(this, SIGNAL(beatDetected()), this, SLOT(triggerEffect()));

    createViewMenu();

    setWidget(groupbox);
}

void audioController::createViewMenu()
{
    viewAudioMenu = new QMenu(tr("&Audio Controller"));

    viewAudioAction = new QAction(tr("&Audio Controller"), this);
    viewAudioAction->setCheckable(true);
    viewAudioAction->setChecked(true);
    connect(viewAudioAction, SIGNAL(toggled(bool)), this, SLOT(setVisible(bool)));
    viewAudioMenu->addAction(viewAudioAction);

    viewAudioGraphAction = new QAction(tr("Audio &Graph"), this);
    viewAudioGraphAction->setCheckable(true);
    viewAudioGraphAction->setChecked(true);
    connect(viewAudioGraphAction, SIGNAL(toggled(bool)), this, SLOT(showGraph(bool)));
    viewAudioMenu->addAction(viewAudioGraphAction);

    viewAudioFFTAction = new QAction(tr("Audio &FFT"), this);
    viewAudioFFTAction->setCheckable(true);
    viewAudioFFTAction->setChecked(true);
    connect(viewAudioFFTAction, SIGNAL(toggled(bool)), this, SLOT(showFFT(bool)));
    viewAudioMenu->addAction(viewAudioFFTAction);

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

void audioController::setSamples(int value)
{
    stopAudio();
    samples = value;
    startAudio();
}

void audioController::startAudio()
{
    sampleSlider->setEnabled(false);
    m_Beat = new libbeat::BeatController(this, samples, AUDIO_INCOMING_SAMPLES_PER_SEC, 192);
    m_fft->setController(m_Beat);
    connect(m_Beat, SIGNAL(beatDrum()), this, SLOT(triggerEffect()));
    connect(m_Beat, SIGNAL(processingDone()), this, SLOT(doReplot()));
    connect(m_Beat, SIGNAL(processingDone()), m_fft, SLOT(drawNewData()));
}


void audioController::stopAudio()
{
    m_fft->setController(NULL);
    delete m_Beat;
    m_Beat = NULL;
    sampleSlider->setEnabled(true);
}

void audioController::showGraph(bool s)
{
    plot->setVisible(s);
}

void audioController::showFFT(bool s)
{
    m_fft->setVisible(s);
}

void audioController::doReplot()
{

    if(m_Beat != NULL && plot->isVisible()){
        libbeat::SoundBuffer *buf = m_Beat->getBuffer();
        int buflen = buf->size();
        int num_samples = buflen / AUDIO_AVERAGE_SAMPLES;

        double *ydata = y.data();

        memmove(ydata+(num_samples-1), ydata, (AUDIO_GRAPH_DISPLAY_SAMPLES-num_samples)*sizeof(double));

        /* Calculate graph data */
        for(int i = 0; i < num_samples; i++){
            double v = 0.0;
            for(int j = 0; j < AUDIO_AVERAGE_SAMPLES; j++){
                v += (double)buf->read(AUDIO_AVERAGE_SAMPLES*i+j);
            }
            ydata[i] = v/AUDIO_AVERAGE_SAMPLES;
        }

        plot->graph(0)->setData(x, y);
        //plot->graph(1)->setData(x, z);

        plot->replot();
    }

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

void audioController::setFFTWindow(const QString value)
{
    if(m_Beat == NULL) return;

    if (value == "None")
        m_Beat->getFFT()->setWindowFunction(libbeat::NO_WINDOW);
    if (value == "Hanning")
        m_Beat->getFFT()->setWindowFunction(libbeat::HANNING);
    if (value == "Blackman")
        m_Beat->getFFT()->setWindowFunction(libbeat::BLACKMAN);
}
