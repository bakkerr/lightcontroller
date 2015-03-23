#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QWidget>
#include <QtGui>

#include "default_values.h"
#include "qcustomplot.h"

enum effects { EFFECT_NO, EFFECT_RANDOM_ALL, EFFECT_RANDOM_SAME, EFFECT_FADE10, EFFECT_FADE20 };

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

class audioController : public QDockWidget
{
    Q_OBJECT

public:
    explicit audioController(QWidget *parent = 0);

signals:
    void beatDetected();
    void setRandomAll();
    void setRandomSame();
    void fade10();
    void fade20();

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
    QRadioButton *noEffectButton;
    QRadioButton *randomSameButton;
    QRadioButton *randomAllButton;
    QRadioButton *fade10Button;
    QRadioButton *fade20Button;

    QGroupBox *settingsBox;
    QSlider *thresholdSlider;
    QLabel *thresholdLabel;
    QSlider *sampleSlider;
    QLabel *sampleLabel;
    QPushButton *manualTrigger;

    QCustomPlot *plot;

    AudioThread *audiothread;
};

#endif // AUDIOCONTROLLER_H
