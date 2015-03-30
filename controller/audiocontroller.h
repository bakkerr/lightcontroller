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

//#include "../../beatdetector/src/soundbuffer.h"

#include "default_values.h"
#include "qcustomplot.h"

enum effects { EFFECT_NO, EFFECT_RANDOM_ALL, EFFECT_RANDOM_SAME, EFFECT_FADE10, EFFECT_FADE20 };
typedef QVector<unsigned char> MyBuffer;

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
    void doReplot(qint64 len);

private slots:
    void triggerEffect();
    void startAudio();
    void stopAudio();
    void readAudio();

private:



    int threshold;
    int samples;
    int lastBeat;

    QGroupBox *groupbox;

    QComboBox *m_deviceBox;
    QAudioInput *m_audioInput;
    QIODevice *m_ioDevice;
    QByteArray *m_buffer;


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
    QVector<double> x, y, z, q;

};

#endif // AUDIOCONTROLLER_H
