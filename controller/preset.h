#ifndef PRESET_H
#define PRESET_H

#include <QtGui>

#include <QObject>
#include <QVector>
#include <QColor>
#include <QString>

#include "default_values.h"
#include "settings.h"

class PresetZone : public QObject
{
    Q_OBJECT

public:
    //explicit PresetZone(QObject *parent = 0);
    explicit PresetZone(bool enabled = true, QColor color = Qt::blue, bool fade = false, int fadeTime = FADE_VALUE_MS_DEFAULT, bool fixed = false, int brightness = BRIGHT_VALUE_DEFAULT, QObject *parent = 0);
    PresetZone(const PresetZone &pz);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    bool equals(PresetZone *a, PresetZone *b);

//private:
    bool m_enabled;
    QColor m_color;
    bool m_fade;
    qint64 m_fadeTime;
    bool m_fixed;
    qint64 m_brightness;

};

class PresetLC : public QObject
{
    Q_OBJECT

public:
    explicit PresetLC(QString id = tr("foo"), QObject *parent = 0);
    PresetLC(const PresetLC &plc);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    PresetZone *zones[5];
    QString m_id;

private:


};

class Preset : public QObject
{
    Q_OBJECT

public:
    explicit Preset(QObject *parent = 0);
    Preset(const Preset &p);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    bool addController(PresetLC *plc);
    void setName(const QString &name);

    QString m_name;
    QDateTime m_date;

//private:
    PresetZone *master;
    QVector<PresetLC*> lcs;


};

#endif // PRESET_H
