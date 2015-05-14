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
    ~PresetZone() { qDebug() << "destruct PZ?" << endl;}
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
    ~PresetLC() {qDebug() << "destruct PLC?" << (quint64)(this) << endl;}

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
    ~Preset() { qDebug() << "destruct P?" << (quint64)(this) << endl;}

    bool addController(PresetLC *plc);
    void setName(const QString &name);

    QString m_name;
    QDateTime m_date;

//private:
    PresetZone *master;
    QVector<PresetLC*> lcs;


};

QDataStream &operator<<(QDataStream &out, const PresetZone &pz);
QDataStream &operator>>(QDataStream &in, PresetZone &pz);
QDataStream &operator<<(QDataStream &out, const PresetLC &plc);
QDataStream &operator>>(QDataStream &in, PresetLC &plc);
QDataStream &operator<<(QDataStream &out, const Preset &p);
QDataStream &operator>>(QDataStream &in, Preset &p);

Q_DECLARE_METATYPE(Preset)


#endif // PRESET_H
