#ifndef PRESET_H
#define PRESET_H

#include <QtGui>

#include <QObject>
#include <QVector>
#include <QColor>
#include <QString>

class PresetZone : public QObject
{
    Q_OBJECT

public:
    //explicit PresetZone(QObject *parent = 0);
    explicit PresetZone(bool enabled, QColor color, bool fade, int fadeTime, bool fixed, int brightness, QObject *parent = 0);
    bool equals(PresetZone *a, PresetZone *b);

//private:
    bool m_enabled;
    QColor m_color;
    bool m_fade;
    int m_fadeTime;
    bool m_fixed;
    int m_brightness;

};

class PresetLC : public QObject
{
    Q_OBJECT

public:
    explicit PresetLC(QString id, QObject *parent = 0);

    PresetZone *zones[5];
    QString m_id;

private:


};

class Preset : public QObject
{
    Q_OBJECT

public:
    explicit Preset(QObject *parent = 0);

    bool addController(PresetLC *plc);

    QString m_name;
    QDateTime m_date;

//private:
    QVector<PresetLC*> lcs;


};

#endif // PRESET_H
