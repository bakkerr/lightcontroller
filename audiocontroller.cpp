#include "audiocontroller.h"

audioController::audioController(QWidget *parent) :
    QWidget(parent)
{
    qRegisterMetaType<MyBuffer>("MyBuffer");

    threshold = AUDIO_THRESHOLD_DEFAULT;
    samples = AUDIO_SAMPLES_DEFAULT;
    lastBeat = 0;
    audiothread = NULL;

    QHBoxLayout *l0 = new QHBoxLayout();

    groupbox = new QGroupBox(tr("Audio"));
    groupbox->setCheckable(true);
    groupbox->setChecked(false);

    QVBoxLayout *l1 = new QVBoxLayout();


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
    settingsBox->setLayout(settingsLayout);
    l1->addWidget(settingsBox);


    effectBox = new QGroupBox(tr("Trigger Effect"));
    QVBoxLayout *effectLayout = new QVBoxLayout();
    effect = new QButtonGroup();
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
    plot->yAxis->setRange(0, 255);
    plot->xAxis->setRange(0, AUDIO_GRAPH_DISPLAY_SAMPLES / (double)AUDIO_INCOMING_SAMPLES_PER_SEC);
    plot->replot();

    l4->addWidget(plot);

    groupbox->setLayout(l4);

    l0->addWidget(groupbox);

    connect(groupbox, SIGNAL(clicked(bool)), this, SLOT(stateChange(bool)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(setThreshold(int)));
    connect(sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSamples(int)));

    connect(this, SIGNAL(beatDetected()), this, SLOT(triggerEffect()));

    this->setLayout(l0);

}

void audioController::stateChange(bool s)
{
    if(s && audiothread == NULL) {
        startAudioThread();
    }
    else{
        stopAudiothread();
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

void audioController::startAudioThread()
{
    audiothread = new AudioThread(this);
    audiothread->start();
}

void audioController::stopAudiothread()
{
    audiothread->setStop();
    //audiothread->terminate();

    audiothread->wait();
    audiothread->quit();

    plot->graph(0)->clearData();
    plot->graph(1)->clearData();
    plot->graph(2)->clearData();
    plot->replot();

    delete audiothread;

    audiothread = NULL;
}

void audioController::doReplot(MyBuffer buffer, int n)
{
    /* Graph data */
    QVector<double> x(AUDIO_GRAPH_DISPLAY_SAMPLES),
                    y(AUDIO_GRAPH_DISPLAY_SAMPLES),
                    z(AUDIO_GRAPH_DISPLAY_SAMPLES),
                    q(AUDIO_GRAPH_DISPLAY_SAMPLES);

    /* Initial buffersize might be smaller */
    int s = buffer.size();

    /* Calculate graph data */
    for(int i = 0; i < s; i++){

        /* Value from buffer (newest last) */
        unsigned char v = buffer.at(i);

        /* Set x-axis data in seconds */
        x[i] = (double)(i) / AUDIO_INCOMING_SAMPLES_PER_SEC;

        /* Set data in reverse direction. (newest first) */
        y[s - 1 - i] = (double)v;

        /* Threshold line (constant) */
        z[i] = threshold;

        /*
         * Draw vertical lines on samples interval
         * FIXME: Convert to detected "beats"
         */
        if(i % samples == 0){
            q[i] = 255;
        }
        else {
            q[i] = 0;
        }

    }

    /* FIXME: When do we want to appove a new beat detect? */
    lastBeat += AUDIO_GRAPH_UPDATE_SAMPLES;

    /*
     * Do "beat" detect
     * FIXME: should do this somehow more advanced (fft?)
     */
    int max = 0;

    /* See if the last detected "beat" is at least more than #samples ago */
    if(lastBeat >= samples + AUDIO_GRAPH_UPDATE_SAMPLES){
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
{ qDebug() << "Trigger!" << effect->checkedId() << EFFECT_NO << EFFECT_RANDOM_SAME << EFFECT_RANDOM_ALL << endl;
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

/*
 * AudioThread constructor
 */
AudioThread::AudioThread(QWidget *parent)
{
    n = 0;
    stop = false;
    command = AUDIO_RECORD_COMMAND_DEFAULT;
    qDebug() << command << endl;
    /*
     * For Linux, we can call the command with popen and
     * get a file descriptor to read the command's output
     */
    stream = popen(command, "r");

    connect(this, SIGNAL(bufferFilled(MyBuffer, int)), parent, SLOT(doReplot(MyBuffer, int)));

}

/*
 * AudioThread destructor
 */
AudioThread::~AudioThread()
{
    /* Close stream. */
    pclose(stream);
}

/*
 * AudioThread mainloop
 */
void AudioThread::run()
{
    /* Keep running until we need to stop. */
    while(!stop){

        /* Get samples from audio input */
        for(int i = 0; i < AUDIO_GRAPH_UPDATE_SAMPLES; i++){
            unsigned char val = fgetc(stream);
            buffer.push_back(val);
        }

        /* Remove oldest values until size is as required. */
        while(buffer.size() >= AUDIO_GRAPH_DISPLAY_SAMPLES){
            buffer.pop_front();
        }

        /* Pass values to the controller (makes a copy of the vector if required) */
        if(!stop){
            emit bufferFilled(buffer, n);
        }
        n++;
    }
}

