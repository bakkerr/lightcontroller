#include "audiocontroller.h"

/* Need to move this */
const int BufferSize = 1000;

/*
 * Audio controller constructor, creates a dockwidget. */
audioController::audioController(QWidget *parent) :
    QDockWidget(tr("Audio Controller"), parent),
    m_plotX(AUDIO_GRAPH_DISPLAY_SAMPLES),
    m_plotY(AUDIO_GRAPH_DISPLAY_SAMPLES),
    m_plotZ(AUDIO_GRAPH_DISPLAY_SAMPLES)
{
    /* Set default. */
    m_samples = AUDIO_SAMPLES_DEFAULT;
    m_beat = NULL;

    m_groupBox = new QGroupBox(tr("Enable"), this);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(false);

    QVBoxLayout *l1 = new QVBoxLayout();

    QLabel *inputLabel = new QLabel(tr("Audio Input:"), this);
    inputLabel->setToolTip(tr("Choose the audio input device."));
    QHBoxLayout *l001 = new QHBoxLayout();

    m_deviceBox = new QComboBox(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    if(defaultDeviceInfo.isNull()){
        this->setEnabled(false);
        this->setToolTip(tr("AudioController has been disabled because it could not find a default audio input device. Please add an input device and restart ") + APPLICATION_NAME);
    }

    /*
     * Create a list of all audio input devices available on the system.
     * Make sure the default one is on top
     */
    m_inputDevice = defaultDeviceInfo;
    m_deviceBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    m_deviceBox->setMaximumWidth(150);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }

    connect(m_deviceBox, SIGNAL(activated(int)), SLOT(inputDeviceChanged(int)));
    l001->addWidget(inputLabel);
    l001->addWidget(m_deviceBox);

    l1->addLayout(l001);

    QHBoxLayout *l4 = new QHBoxLayout();

    m_settingsBox = new QGroupBox(tr("Trigger Settings"), this);
    m_settingsBox->setMaximumWidth(250);
    QVBoxLayout *settingsLayout = new QVBoxLayout();

    QHBoxLayout *sampleLayout = new QHBoxLayout();
    m_sampleSlider = new QSlider(Qt::Horizontal, this);
    m_sampleSlider->setMinimum(AUDIO_SAMPLES_MIN);
    m_sampleSlider->setMaximum(AUDIO_SAMPLES_MAX);
    m_sampleSlider->setValue(m_samples);
    m_sampleSlider->setMaximumWidth(150);
    m_sampleLabel = new QLabel(tr("Samples:"), this);
    m_sampleLabel->setToolTip(tr("Set the number of samples to use for the FFT window"));
    m_sampleLabel->setMaximumWidth(100);
    sampleLayout->addWidget(m_sampleLabel);
    sampleLayout->addWidget(m_sampleSlider);
    settingsLayout->addLayout(sampleLayout);

    QHBoxLayout *fftWindowLayout = new QHBoxLayout();
    m_fftWindowBox = new QComboBox(this);
    m_fftWindowLabel = new QLabel(tr("FFT Window:"), this);
    m_fftWindowLabel->setToolTip(tr("Set the FFT window to use."));
    m_fftWindowBox->addItem(tr("None"));
    m_fftWindowBox->addItem(tr("Hanning"));
    m_fftWindowBox->addItem(tr("Blackman"));
    connect(m_fftWindowBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFFTWindow(QString)));
    fftWindowLayout->addWidget(m_fftWindowLabel);
    fftWindowLayout->addWidget(m_fftWindowBox);
    settingsLayout->addLayout(fftWindowLayout);

    QHBoxLayout *triggerControllerLayout = new QHBoxLayout();
    QLabel *triggerControllerLabel = new QLabel("Controller:");
    triggerControllerLayout->addWidget(triggerControllerLabel);
    m_triggerControllerBox = new QComboBox(this);
    updateControllers();
    connect(m_triggerControllerBox, SIGNAL(currentIndexChanged(int)), SIGNAL(controllerChanged(int)));
    triggerControllerLayout->addWidget(m_triggerControllerBox);
    settingsLayout->addLayout(triggerControllerLayout);

    m_manualTriggerButton = new QPushButton(tr("Trigger Manual"), this);
    connect(m_manualTriggerButton, SIGNAL(clicked()), this, SLOT(triggerEffect()));
    settingsLayout->addWidget(m_manualTriggerButton);
    m_settingsBox->setLayout(settingsLayout);
    l1->addWidget(m_settingsBox);

    m_effectBox = new QGroupBox(tr("Trigger Effect"), this);
    QHBoxLayout *effectLayout0 = new QHBoxLayout();
    QVBoxLayout *effectLayout1 = new QVBoxLayout();
    QVBoxLayout *effectLayout2 = new QVBoxLayout();
    m_effectGroup = new QButtonGroup(this);
    m_effectGroup->setExclusive(true);
    m_noEffectButton = new QRadioButton(tr("None"), this);
    m_randomSameButton = new QRadioButton(tr("Same random"), this);
    m_randomSameButton->setChecked(true);
    m_randomAllButton = new QRadioButton(tr("Different random"), this);
    m_flashButton = new QRadioButton(tr("Flash"), this);
    m_flashRandomButton = new QRadioButton(tr("Flash + Random"), this);
    m_fade10Button = new QRadioButton(tr("Fade +10"), this);
    m_fade20Button = new QRadioButton(tr("Fade +20"), this);
    m_effectGroup->addButton(m_noEffectButton, EFFECT_NO);
    m_effectGroup->addButton(m_randomSameButton, EFFECT_RANDOM_SAME);
    m_effectGroup->addButton(m_randomAllButton, EFFECT_RANDOM_ALL);
    m_effectGroup->addButton(m_flashButton, EFFECT_FLASH);
    m_effectGroup->addButton(m_flashRandomButton, EFFECT_FLASH_RANDOM);
    m_effectGroup->addButton(m_fade10Button, EFFECT_FADE10);
    m_effectGroup->addButton(m_fade20Button, EFFECT_FADE20);
    effectLayout1->addWidget(m_noEffectButton);
    effectLayout1->addWidget(m_randomSameButton);
    effectLayout1->addWidget(m_randomAllButton);
    effectLayout1->addWidget(m_flashRandomButton);
    effectLayout2->addWidget(m_flashButton);
    effectLayout2->addWidget(m_fade10Button);
    effectLayout2->addWidget(m_fade20Button);
    effectLayout0->addLayout(effectLayout1);
    effectLayout0->addLayout(effectLayout2);
    m_effectBox->setLayout(effectLayout0);
    l1->addWidget(m_effectBox);

    l4->addLayout(l1);

    QVBoxLayout *l5 = new QVBoxLayout();

    m_plot = new QCustomPlot(this);
    m_plot->setToolTip(tr("Raw audio data view"));
    m_plot->setMinimumWidth(200);
    m_plot->addGraph();
    m_plot->addGraph();
    m_plot->graph(1)->setPen(QPen(Qt::green));

    for(int i = 0; i < AUDIO_GRAPH_DISPLAY_SAMPLES; i++){
        m_plotX[i] = (double)(i) / (AUDIO_INCOMING_SAMPLES_PER_SEC / AUDIO_AVERAGE_SAMPLES);
    }

    m_plot->setBackground(m_groupBox->palette().background());
    m_plot->yAxis->setRange(-30000, 30000);
    m_plot->yAxis->setTickLabels(false);
    m_plot->xAxis->setRange(0, AUDIO_GRAPH_DISPLAY_SAMPLES / ((double)AUDIO_INCOMING_SAMPLES_PER_SEC/AUDIO_AVERAGE_SAMPLES));

    m_plot->replot();

    l5->addWidget(m_plot);

    m_fft = new FFTDisplay(NULL, this);
    m_fft->setToolTip(tr("FFT output: Volume bar (right), Triggers [Beat: yellow, Snare blue] (circles), Audio values (bottom), Triggers for each FFT band (above)"));
    m_fft->setMinimumWidth(200);
    l5->addWidget(m_fft);

    l4->addLayout(l5);

    m_groupBox->setLayout(l4);

    connect(m_groupBox, SIGNAL(toggled(bool)), this, SLOT(stateChange(bool)));
    connect(m_sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));

    connect(this, SIGNAL(beatDetected()), this, SLOT(triggerEffect()));

    createViewMenu();

    setWidget(m_groupBox);
}

void audioController::createViewMenu()
{
    viewAudioMenu = new QMenu(tr("&Audio Controller"), this);
    viewAudioMenu->addAction(toggleViewAction());
    viewAudioMenu->addSeparator();

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

void audioController::loadSettings(QSettings *s)
{
    s->beginGroup("AudioController");
    setVisible(s->value(tr("visible"), tr("true")).toBool());
    viewAudioGraphAction->setChecked(s->value(tr("displayGraph"), tr("true")).toBool());
    viewAudioFFTAction->setChecked(s->value(tr("displayFFT"), tr("true")).toBool());
    m_groupBox->setChecked(s->value(tr("enabled"), tr("false")).toBool());
    m_sampleSlider->setValue(s->value(tr("samples"), AUDIO_SAMPLES_DEFAULT).toInt());
    m_fftWindowBox->setCurrentText(s->value(tr("fftWindow"), tr("None")).toString());
    m_triggerControllerBox->setCurrentText(s->value(tr("controller"), tr("<None>")).toString());
    m_effectGroup->button(s->value(tr("effect"), 0).toInt())->setChecked(true);

    s->endGroup();
}

void audioController::saveSettings(QSettings *s)
{
    s->beginGroup("AudioController");
    s->setValue(tr("visible"), isVisible());
    s->setValue(tr("displayGraph"), viewAudioGraphAction->isChecked());
    s->setValue(tr("displayFFT"), viewAudioFFTAction->isChecked());
    s->setValue(tr("enabled"), m_groupBox->isChecked());
    s->setValue(tr("samples"), m_samples);
    s->setValue(tr("fftWindow"), m_fftWindowBox->currentText());
    s->setValue(tr("controller"), m_triggerControllerBox->currentText());
    s->setValue(tr("effect"), m_effectGroup->checkedId());
    s->endGroup();
}

void audioController::updateControllers(QList<SingleController *> controllers)
{
    m_triggerControllerBox->blockSignals(true);
    m_triggerControllerBox->clear();
    m_triggerControllerBox->addItem(tr("<None>"), 0x00);
    foreach(SingleController *lc, controllers){
        QString remoteString = QString::number(lc->remote(), 16);
        while(remoteString.length() < 4){
            remoteString.prepend("0");
        }
        m_triggerControllerBox->addItem(lc->name() + tr(" (") + remoteString + tr(")"), lc->id());
    }
    m_triggerControllerBox->blockSignals(false);
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
    m_samples = value;

    if(m_beat == NULL) return;

    stopAudio();
    startAudio();
}

void audioController::startAudio()
{
    m_beat = new libbeat::BeatController(m_inputDevice, m_samples, AUDIO_INCOMING_SAMPLES_PER_SEC, 192, this);
    m_fft->setController(m_beat);
    connect(m_beat, SIGNAL(beatDrum()), this, SLOT(triggerEffect()));
    connect(m_beat, SIGNAL(processingDone()), this, SLOT(doReplot()));
    connect(m_beat, SIGNAL(processingDone()), m_fft, SLOT(drawNewData()));
    setFFTWindow(m_fftWindowBox->currentText());
}


void audioController::stopAudio()
{
    m_fft->setController(NULL);
    delete m_beat;
    m_beat = NULL;
}

void audioController::showGraph(bool s)
{
    m_plot->setVisible(s);
    GLOBAL_settingsChanged = true;
}

void audioController::showFFT(bool s)
{
    m_fft->setVisible(s);
    GLOBAL_settingsChanged = true;
}

void audioController::doReplot()
{

    if(m_beat != NULL && m_plot->isVisible()){
        libbeat::SoundBuffer *buf = m_beat->getBuffer();
        int buflen = buf->size();
        int num_samples = buflen / AUDIO_AVERAGE_SAMPLES;

        double *ydata = m_plotY.data();

        memmove(ydata+(num_samples-1), ydata, (AUDIO_GRAPH_DISPLAY_SAMPLES-num_samples)*sizeof(double));

        /* Calculate graph data */
        for(int i = 0; i < num_samples; i++){
            double v = 0.0;
            for(int j = 0; j < AUDIO_AVERAGE_SAMPLES; j++){
                v += (double)buf->read(AUDIO_AVERAGE_SAMPLES*i+j);
            }
            ydata[i] = v/AUDIO_AVERAGE_SAMPLES;
        }

        m_plot->graph(0)->setData(m_plotX, m_plotY);

        m_plot->replot();
    }

}

void audioController::triggerEffect()
{
    switch(m_effectGroup->checkedId()){
        case EFFECT_RANDOM_ALL:
            emit setRandomDifferent();
            break;
        case EFFECT_RANDOM_SAME:
            emit setRandomSame();
            break;
        case EFFECT_FLASH:
            emit flash();
            break;
        case EFFECT_FLASH_RANDOM:
            emit flashRandom();
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

void audioController::inputDeviceChanged(int dev)
{
    stopAudio();
    m_inputDevice = m_deviceBox->itemData(dev).value<QAudioDeviceInfo>();
    startAudio();
}

void audioController::setFFTWindow(const QString value)
{
    if(m_beat == NULL) return;

    if (value == "None")
        m_beat->getFFT()->setWindowFunction(libbeat::NO_WINDOW);
    if (value == "Hanning")
        m_beat->getFFT()->setWindowFunction(libbeat::HANNING);
    if (value == "Blackman")
        m_beat->getFFT()->setWindowFunction(libbeat::BLACKMAN);
}
