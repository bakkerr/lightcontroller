#ifndef PRESET_H
#define PRESET_H

#include <QtGui>

#include <QObject>
#include <QVector>
#include <QColor>
#include <QString>

#include "default_values.h"
#include "settings.h"

class PresetLC : public QObject
{
    Q_OBJECT

public:
    explicit PresetLC(quint16 id, bool enabled = true, QColor color = Qt::blue, bool fade = false, int fadeTime = FADE_VALUE_MS_DEFAULT, bool fixed = false, int brightness = BRIGHT_VALUE_DEFAULT, QObject *parent = 0);
    PresetLC(const PresetLC &pz);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    bool equals(PresetLC *a, PresetLC *b);

    quint16 id() { return m_id; };

    quint16 m_id;
    bool m_enabled;
    QColor m_color;
    bool m_fade;
    qint64 m_fadeTime;
    bool m_fixed;
    qint64 m_brightness;

};

class Preset : public QObject
{
    Q_OBJECT

public:
    explicit Preset(QObject *parent = 0);
    Preset(const Preset &p);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

    bool addController(PresetLC *pc);
    void setName(const QString &name);

    QString m_name;
    QDateTime m_date;

    QVector<PresetLC*> lcs;

};

#endif // PRESET_H
