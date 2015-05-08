#include "audiocontroller.h"

const int BufferSize = 1000;


audioController::audioController(QWidget *parent) :
    QDockWidget(tr("Audio Controller"), parent),
    m_plotX(AUDIO_GRAPH_DISPLAY_SAMPLES),
    m_plotY(AUDIO_GRAPH_DISPLAY_SAMPLES),
    m_plotZ(AUDIO_GRAPH_DISPLAY_SAMPLES)
{
    m_samples = AUDIO_SAMPLES_DEFAULT;

    m_groupBox = new QGroupBox(tr("Enable"), this);
    m_groupBox->setCheckable(true);
    m_groupBox->setChecked(false);

    QVBoxLayout *l1 = new QVBoxLayout();

    QGroupBox *inputDevice = new QGroupBox(tr("Audio Input"), this);
    QHBoxLayout *l001 = new QHBoxLayout();

    m_deviceBox = new QComboBox(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    m_inputDevice = defaultDeviceInfo;
    m_deviceBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    m_deviceBox->setMaximumWidth(150);
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_deviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }
    //m_deviceBox->setEnabled(false);

    connect(m_deviceBox, SIGNAL(activated(int)), SLOT(inputDeviceChanged(int)));
    l001->addWidget(m_deviceBox);
    inputDevice->setLayout(l001);

    l1->addWidget(inputDevice);

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
    m_sampleLabel->setMaximumWidth(100);
    sampleLayout->addWidget(m_sampleLabel);
    sampleLayout->addWidget(m_sampleSlider);
    settingsLayout->addLayout(sampleLayout);

    QHBoxLayout *fftWindowLayout = new QHBoxLayout();
    m_fftWindowBox = new QComboBox(this);
    m_fftWindowLabel = new QLabel(tr("FFT Window:"), this);
    m_fftWindowBox->addItem(tr("None"));
    m_fftWindowBox->addItem(tr("Hanning"));
    m_fftWindowBox->addItem(tr("Blackman"));
    connect(m_fftWindowBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setFFTWindow(QString)));
    fftWindowLayout->addWidget(m_fftWindowLabel);
    fftWindowLayout->addWidget(m_fftWindowBox);
    settingsLayout->addLayout(fftWindowLayout);

    m_manualTriggerButton = new QPushButton(tr("Trigger Manual"), this);
    connect(m_manualTriggerButton, SIGNAL(clicked()), this, SLOT(triggerEffect()));
    settingsLayout->addWidget(m_manualTriggerButton);
    m_settingsBox->setLayout(settingsLayout);
    l1->addWidget(m_settingsBox);

    m_effectBox = new QGroupBox(tr("Trigger Effect"), this);
    QVBoxLayout *effectLayout = new QVBoxLayout();
    m_effectGroup = new QButtonGroup(this);
    m_effectGroup->setExclusive(true);
    m_noEffectButton = new QRadioButton(tr("None"), this);
    m_randomSameButton = new QRadioButton(tr("Same random"), this);
    m_randomSameButton->setChecked(true);
    m_randomAllButton = new QRadioButton(tr("Different random"), this);
    m_fade10Button = new QRadioButton(tr("Fade +10"), this);
    m_fade20Button = new QRadioButton(tr("Fade +20"), this);
    m_effectGroup->addButton(m_noEffectButton, EFFECT_NO);
    m_effectGroup->addButton(m_randomSameButton, EFFECT_RANDOM_SAME);
    m_effectGroup->addButton(m_randomAllButton, EFFECT_RANDOM_ALL);
    m_effectGroup->addButton(m_fade10Button, EFFECT_FADE10);
    m_effectGroup->addButton(m_fade20Button, EFFECT_FADE20);
    effectLayout->addWidget(m_noEffectButton);
    effectLayout->addWidget(m_randomSameButton);
    effectLayout->addWidget(m_randomAllButton);
    effectLayout->addWidget(m_fade10Button);
    effectLayout->addWidget(m_fade20Button);
    m_effectBox->setLayout(effectLayout);
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
    l5->addWidget(m_fft);

    l4->addLayout(l5);

    m_groupBox->setLayout(l4);

    connect(m_groupBox, SIGNAL(clicked(bool)), this, SLOT(stateChange(bool)));
    connect(m_sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));

    connect(this, SIGNAL(beatDetected()), this, SLOT(triggerEffect()));

    createViewMenu();

    setWidget(m_groupBox);
}

void audioController::createViewMenu()
{
    viewAudioMenu = new QMenu(tr("&Audio Controller"), this);

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
    m_samples = value;
    startAudio();
}

void audioController::startAudio()
{
    //m_sampleSlider->setEnabled(false);
    m_beat = new libbeat::BeatController(m_inputDevice, m_samples, AUDIO_INCOMING_SAMPLES_PER_SEC, 192, this);
    m_fft->setController(m_beat);
    connect(m_beat, SIGNAL(beatDrum()), this, SLOT(triggerEffect()));
    connect(m_beat, SIGNAL(processingDone()), this, SLOT(doReplot()));
    connect(m_beat, SIGNAL(processingDone()), m_fft, SLOT(drawNewData()));
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
}

void audioController::showFFT(bool s)
{
    m_fft->setVisible(s);
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
        //plot->graph(1)->setData(x, z);

        m_plot->replot();
    }

}

void audioController::triggerEffect()
{
    switch(m_effectGroup->checkedId()){
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
