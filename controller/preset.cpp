#include "preset.h"

/*PresetZone::PresetZone(QObject *parent)

{
   m_enabled = false;
}*/

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

PresetLC::PresetLC(QObject *parent) :
    QObject(parent)

{
    //cout << "PLC" << endl;
}

Preset::Preset(QObject *parent) :
    QObject(parent)
{
    m_date = QDateTime::currentDateTime();
    //cout << "Preset" << endl;
}

bool Preset::addController(PresetLC *plc)
{
    lcs.append(plc);

    return true;
}

