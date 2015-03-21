#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QWidget>
#include <QtGui>

#include "config.h"
#include "qcustomplot.h"

enum effects { EFFECT_NO, EFFECT_RANDOM_ALL, EFFECT_RANDOM_SAME };

typedef QVector<unsigned char> MyBuffer;

class AudioThread : public QThread
{
    Q_OBJECT
public:
    AudioThread(QWidget *parent = 0);
    ~AudioThread();

public slots:
    void setStop() { stop = true; }

signals:
    void bufferFilled(MyBuffer, int n);

private:
    void run();

    MyBuffer buffer;
    char *command;
    FILE *stream;
    int n;
    bool stop;
};

class audioController : public QWidget
{
    Q_OBJECT

public:
    explicit audioController(QWidget *parent = 0);

signals:
    void beatDetected();
    void setRandomAll();
    void setRandomSame();

public slots:
    void stateChange(bool s);
    void setThreshold(int value);
    void setSamples(int value);
    void doReplot(MyBuffer buffer, int n);

private slots:
    void triggerEffect();

private:
    void startAudioThread();
    void stopAudiothread();

    int threshold;
    int samples;
    int lastBeat;

    QGroupBox *groupbox;

    QGroupBox *effectBox;
    QButtonGroup *effect;
    QRadioButton *noEffect;
    QRadioButton *randomSame;
    QRadioButton *randomAll;

    QGroupBox *settingsBox;
    QSlider *thresholdSlider;
    QLabel *thresholdLabel;
    QSlider *sampleSlider;
    QLabel *sampleLabel;
    QCustomPlot *plot;

    AudioThread *audiothread;
};

#endif // AUDIOCONTROLLER_H
