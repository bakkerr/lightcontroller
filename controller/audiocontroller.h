#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QDockWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QAudioInput>

#include "../libbeat/beatcontroller.h"
#include "../libbeat/soundbuffer.h"

#include "default_values.h"
#include "qcustomplot.h"

enum effects { EFFECT_NO, EFFECT_RANDOM_ALL, EFFECT_RANDOM_SAME, EFFECT_FADE10, EFFECT_FADE20 };

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
    void setSamples(int value);
    void doReplot();

private slots:
    void triggerEffect();
    void startAudio();
    void stopAudio();

private:
    int samples;
    int lastBeat;

    QGroupBox *groupbox;

    QComboBox *m_deviceBox;

    libbeat::BeatController *m_Beat;

    QGroupBox *effectBox;
    QButtonGroup *effect;
    QRadioButton *noEffectButton;
    QRadioButton *randomSameButton;
    QRadioButton *randomAllButton;
    QRadioButton *fade10Button;
    QRadioButton *fade20Button;

    QGroupBox *settingsBox;
    QSlider *sampleSlider;
    QLabel *sampleLabel;
    QPushButton *manualTrigger;

    QCustomPlot *plot;
    QVector<double> x, y, z;

};

#endif // AUDIOCONTROLLER_H
