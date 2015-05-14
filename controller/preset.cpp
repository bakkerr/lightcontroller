#include "preset.h"

/*PresetZone::PresetZone(QObject *parent)

{
   m_enabled = false;
}*/

PresetZone::PresetZone(bool enabled, QColor color, bool fade, int fadeTime, bool fixed, int brightness, QObject *parent) :
    QObject(parent)
{
    qDebug() << "Construct pz" << (quint64)(this) << endl;
    m_enabled = enabled;
    m_color = color;
    m_fade = fade;
    m_fadeTime = fadeTime;
    m_fixed = fixed;
    m_brightness = brightness;
}

PresetZone::PresetZone(const PresetZone &pz) :
    QObject()
{
    qDebug() << "Construct pz(c)" << (quint64)(this) << endl;
    m_enabled = pz.m_enabled;
    m_color = pz.m_color;
    m_fade = pz.m_fade;
    m_fadeTime = pz.m_fadeTime;
    m_fixed = pz.m_fixed;
    m_brightness = pz.m_brightness;
}


bool PresetZone::equals(PresetZone *a, PresetZone *b)
{
    return(
      (a->m_enabled    == b->m_enabled)    &&
      (a->m_color      == b->m_color)      &&
      (a->m_fade       == b->m_fade)       &&
      (a->m_fadeTime   == b->m_fadeTime)   &&
      (a->m_fixed      == b->m_fixed)      &&
      (a->m_brightness == b->m_brightness) );
}

PresetLC::PresetLC(QString id, QObject *parent) :
    QObject(parent)
{
    qDebug() << "Construct plc" << (quint64)(this) << endl;
    m_id = id;
    //cout << "PLC" << endl;
}

PresetLC::PresetLC(const PresetLC &plc) :
    QObject()
{
    qDebug() << "Construct plc(c)" << (quint64)(this) << endl;
    m_id = plc.m_id;
    for(int i = 0; i < 5; i++){
        zones[i] = new PresetZone(plc.zones[i]);
    }
}

Preset::Preset(QObject *parent) :
    QObject(parent)
{qDebug() << "Construct p" << (quint64)(this) << endl;
    m_name = QString();
    m_date = QDateTime::currentDateTime();
    //cout << "Preset" << endl;
}

Preset::Preset(const Preset &p) :
    QObject()
{ qDebug() << "Construct p(c)" << (quint64)(this) << endl;
    m_date = p.m_date;
    m_name = p.m_name;
    master = p.master;
    lcs = p.lcs;
}

bool Preset::addController(PresetLC *plc)
{
    plc->setParent(this);
    lcs.append(plc);

    return true;
}

void Preset::setName(const QString &name)
{
    if(m_name == name) return;

    m_name = name;

    GLOBAL_settingsChanged = true;
}

QDataStream &operator<<(QDataStream &out, const PresetZone *pz)
{
    out << pz->m_enabled;
    out << pz->m_color;
    out << pz->m_fade;
    out << pz->m_fadeTime;
    out << pz->m_fixed;
    out << pz->m_brightness;

    return out;
}

QDataStream &operator>>(QDataStream &in, PresetZone *pz)
{
    in >> pz->m_enabled;
    in >> pz->m_color;
    in >> pz->m_fade;
    in >> pz->m_fadeTime;
    in >> pz->m_fixed;
    in >> pz->m_brightness;

    return in;
}

QDataStream &operator<<(QDataStream &out, const PresetLC *plc)
{
    out << plc->m_id;
    for(int i = 0; i < 5; i++){
        out << plc->zones[i];
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, PresetLC *plc)
{
    in >> plc->m_id;
    for(int i = 0; i < 5; i++){
        PresetZone *pz = new PresetZone();
        in >> pz;
        plc->zones[i] = pz;
    }

    return in;
}



QDataStream &operator>>(QDataStream &in, Preset *p){
    int size = 0;

    in >> p->m_name;
    in >> p->m_date;
    PresetZone *pz = new PresetZone();
    in >> pz;
    p->master = pz;
    in >> size;
    for(int i = 0; i < size; i++){
        PresetLC *plc = new PresetLC();
        in >> plc;
        p->addController(plc);
    }

    return in;
}

QDataStream &operator<<(QDataStream &out, const Preset *p)
{
    out << p->m_name;
    out << p->m_date;
    out << p->master;
    out << p->lcs.size();
    for(int i = 0; i < p->lcs.size(); i++){
        out << p->lcs.at(i);
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, Preset **p){
    *p = new Preset();
    in >> p;

    return in;
}


QDataStream &operator<<(QDataStream &out, const Preset **p)
{
    return out;
}

