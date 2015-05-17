#include "preset.h"

PresetZone::PresetZone(bool enabled, QColor color, bool fade, int fadeTime, bool fixed, int brightness, QObject *parent) :
    QObject(parent)
{
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
    m_enabled = pz.m_enabled;
    m_color = pz.m_color;
    m_fade = pz.m_fade;
    m_fadeTime = pz.m_fadeTime;
    m_fixed = pz.m_fixed;
    m_brightness = pz.m_brightness;
}

void PresetZone::loadSettings(QSettings *s){
    m_enabled = s->value(tr("enabled"), tr("true")).toBool();
    m_color = s->value(tr("color"), QVariant(QColor(Qt::blue))).value<QColor>();
    m_fade = s->value(tr("fade"), tr("false")).toBool();
    m_fadeTime = s->value(tr("fadeTime"), tr("10000")).toInt();
    m_fixed = s->value(tr("fixed"), tr("false")).toBool();
    m_brightness = s->value(tr("brightness"), tr("18")).toInt();
}

void PresetZone::saveSettings(QSettings *s)
{
    s->setValue(tr("enabled"), m_enabled);
    s->setValue(tr("color"), m_color);
    s->setValue(tr("fade"), m_fade);
    s->setValue(tr("fadeTime"), m_fadeTime);
    s->setValue(tr("fixed"), m_fixed);
    s->setValue(tr("brightness"), m_brightness);
}

PresetLC::PresetLC(QString id, QObject *parent) :
    QObject(parent)
{   
    m_id = id;
}

PresetLC::PresetLC(const PresetLC &plc) :
    QObject()
{
    m_id = plc.m_id;
    for(int i = 0; i < 5; i++){
        zones[i] = new PresetZone(plc.zones[i]);
    }
}

void PresetLC::loadSettings(QSettings *s)
{
    m_id = s->value(tr("id"), tr("ABCD")).toString();
    int size = s->beginReadArray(tr("zones"));
    if(size != 5) abort();
    for(int i = 0; i < 5; i++){
        zones[i] = new PresetZone(this);
        zones[i]->loadSettings(s);
    }
    s->endArray();
}

void PresetLC::saveSettings(QSettings *s)
{
    s->setValue(tr("id"), m_id);
    s->beginWriteArray(tr("zones"));
    for(int i = 0; i < 5; i++){
        s->setArrayIndex(i);
        zones[i]->saveSettings(s);
    }
    s->endArray();
}

Preset::Preset(QObject *parent) :
    QObject(parent)
{
    m_name = QString();
    m_date = QDateTime::currentDateTime();
}

Preset::Preset(const Preset &p) :
    QObject()
{
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

void Preset::loadSettings(QSettings *s)
{
    m_name = s->value(tr("name"), tr("ABCD")).toString();
    m_date = s->value(tr("date"), QVariant(QDateTime::currentDateTime())).value<QDateTime>();
    int size = s->beginReadArray(tr("lightcontrollers"));
    for(int i = 0; i < size; i++){
        s->setArrayIndex(i);
        PresetLC *plc = new PresetLC(tr("ABCD"), this);
        plc->loadSettings(s);
        addController(plc);
    }
    s->endArray();
}

void Preset::saveSettings(QSettings *s)
{
    s->setValue(tr("name"), m_name);
    s->setValue(tr("date"), m_date);
    s->beginWriteArray(tr("lightcontrollers"));
    for(int i = 0; i < lcs.size(); i++){
        s->setArrayIndex(i);
        lcs.at(i)->saveSettings(s);
    }
    s->endArray();
}

void Preset::setName(const QString &name)
{
    if(m_name == name) return;

    m_name = name;

    GLOBAL_settingsChanged = true;
}
