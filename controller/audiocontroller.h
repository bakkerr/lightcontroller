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
#include "fftdisplay.h"

enum effects { EFFECT_NO, EFFECT_RANDOM_ALL, EFFECT_RANDOM_SAME, EFFECT_FADE10, EFFECT_FADE20 };

class audioController : public QDockWidget
{
    Q_OBJECT

public:
    explicit audioController(QWidget *parent = 0);
    QMenu *viewAudioMenu;
    QAction *viewAudioAction;
    QAction *viewAudioGraphAction;
    QAction *viewAudioFFTAction;

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
    void showGraph(bool s);
    void showFFT(bool s);

private slots:
    void inputDeviceChanged(int dev);
    void setFFTWindow(QString value);
    void triggerEffect();
    void startAudio();
    void stopAudio();

private:
    void createViewMenu();
    int m_samples;
    QAudioDeviceInfo m_inputDevice;

    QGroupBox *m_groupBox;

    QComboBox *m_deviceBox;

    libbeat::BeatController *m_beat;

    QGroupBox *m_effectBox;
    QButtonGroup *m_effectGroup;
    QRadioButton *m_noEffectButton;
    QRadioButton *m_randomSameButton;
    QRadioButton *m_randomAllButton;
    QRadioButton *m_fade10Button;
    QRadioButton *m_fade20Button;

    QGroupBox *m_settingsBox;
    QSlider *m_sampleSlider;
    QLabel *m_sampleLabel;
    QPushButton *m_manualTriggerButton;

    QCustomPlot *m_plot;
    FFTDisplay *m_fft;
    QLabel *m_fftWindowLabel;
    QComboBox *m_fftWindowBox;
    QVector<double> m_plotX, m_plotY, m_plotZ;

};

#endif // AUDIOCONTROLLER_H
