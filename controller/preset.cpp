#include "preset.h"

PresetLC::PresetLC(quint16 id = 0x00, bool enabled, QColor color, bool fade, int fadeTime, bool fixed, int brightness, QObject *parent) :
    QObject(parent)
{
    m_id = id;
    m_enabled = enabled;
    m_color = color;
    m_fade = fade;
    m_fadeTime = fadeTime;
    m_fixed = fixed;
    m_brightness = brightness;
}

void PresetLC::loadSettings(QSettings *s){
    m_id = s->value(tr("id"), 0x00).toUInt();
    m_enabled = s->value(tr("enabled"), tr("true")).toBool();
    m_color = s->value(tr("color"), QVariant(QColor(Qt::blue))).value<QColor>();
    m_fade = s->value(tr("fade"), tr("false")).toBool();
    m_fadeTime = s->value(tr("fadeTime"), tr("10000")).toInt();
    m_fixed = s->value(tr("fixed"), tr("false")).toBool();
    m_brightness = s->value(tr("brightness"), tr("18")).toInt();
}

void PresetLC::saveSettings(QSettings *s)
{
    s->setValue(tr("id"), m_id);
    s->setValue(tr("enabled"), m_enabled);
    s->setValue(tr("color"), m_color);
    s->setValue(tr("fade"), m_fade);
    s->setValue(tr("fadeTime"), m_fadeTime);
    s->setValue(tr("fixed"), m_fixed);
    s->setValue(tr("brightness"), m_brightness);
}


Preset::Preset(QObject *parent) :
    QObject(parent)
{
    m_name = QString();
    m_date = QDateTime::currentDateTime();
}

bool Preset::addController(PresetLC *plc)
{
    plc->setParent(this);
    lcs.append(plc);

    return true;
}

void Preset::loadSettings(QSettings *s)
{
    m_name = s->value(tr("name"), tr("Preset")).toString();
    m_date = s->value(tr("date"), QVariant(QDateTime::currentDateTime())).value<QDateTime>();
    int size = s->beginReadArray(tr("Controllers"));
    for(int i = 0; i < size; i++){
        s->setArrayIndex(i);
        PresetLC *plc = new PresetLC();
        plc->setParent(this);
        plc->loadSettings(s);
        addController(plc);
    }
    s->endArray();
}

void Preset::saveSettings(QSettings *s)
{
    s->setValue(tr("name"), m_name);
    s->setValue(tr("date"), m_date);
    s->beginWriteArray(tr("Controllers"));
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
