#include "milightupdsender.h"

MiLightUPDsender::MiLightUPDsender(QString ip, int port, QObject *parent) :
    QObject(parent)
{
    m_addr = QHostAddress(ip);

    if(m_addr.isNull()){
        error("Hostaddress (ip) is invalid");
        exit(EXIT_FAILURE);
    }

    m_udpSocket = new QUdpSocket(this);
    m_port = (quint16)port;

    m_udpResends = UDP_RESENDS_DEFAULT;
    m_resends    = WIRELESS_RESENDS_DEFAULT;
}

void MiLightUPDsender::error(const char* x){
    printf("%s\n", x);
}

void MiLightUPDsender::udpsend(quint16 remote, quint8 color, quint8 bright, quint8 key, quint8 prefix)
{
    const unsigned char command[7] = {(unsigned char)prefix, (unsigned char)(remote >> 8), (unsigned char)(remote & 0xFF),
                             (unsigned char)color,  (unsigned char)bright, (unsigned char)key, (unsigned char)m_resends};

    //for(int i = 0; i < 7; i++) printf("%2x ", command[i]);
    //printf("\n"); fflush(stdout);

    for(int i = 0; i < m_udpResends; i++){
        qint64 bs = m_udpSocket->writeDatagram((char*)command, 7, m_addr, m_port);
        if(bs <= 0) error("Error sending data!\n");
    }

}

void MiLightUPDsender::setColor(const QColor &c, quint16 zone)
{
    int color = 24 + ((int)(c.hue() / 360.0 * 255.0)) % 256;
    udpsend(zone, color, 0x00, 0x0F);
}

void MiLightUPDsender::setBright(unsigned char value, quint16 zone)
{
    unsigned char BRIGHTcodes[19] = {0x80, 0x78, 0x70, 0x68, 0x50,
                            0x48, 0x40, 0x38, 0x28, 0x20,
                            0x18, 0x10, 0x00, 0xF8, 0xF0,
                            0xE8, 0xD8, 0xD0, 0xC8};

    if(value < 19)
    {
        udpsend(zone, 0x00, BRIGHTcodes[value], 0x0E);
    }
}

void MiLightUPDsender::setOn(quint16 zone)
{
    udpsend(zone, 0x00, 0x00, 0x01);
}

void MiLightUPDsender::setOff(quint16 zone)
{
    udpsend(zone, 0x00, 0x00, 0x02);
}

void MiLightUPDsender::setWhite(quint16 zone)
{
    udpsend(zone, 0x00, 0x00, 0x11);
}

void MiLightUPDsender::setNight(quint16 zone)
{
    udpsend(zone, 0x00, 0x00, 0x12);
}

void MiLightUPDsender::setBuildinEffect(quint8 effect, quint16 zone)
{
    if(effect <= 8){
      udpsend(zone, 0x00, 0x00, 0x0D, effect + 0xB0);
    }
}

void MiLightUPDsender::increaseSpeed(quint16 zone, quint8 effect)
{
    udpsend(zone, 0x00, 0x00, 0xB, effect);
}

void MiLightUPDsender::decreaseSpeed(quint16 zone, quint8 effect)
{
    udpsend(zone, 0x00, 0x00, 0xC, effect);
}

void MiLightUPDsender::pair(quint16 zone){
    udpsend(zone, 0x00, 0x00, 0x03);
}

void MiLightUPDsender::unPair(quint16 zone){
    udpsend(zone, 0x00, 0x00, 0x03);
    udpsend(zone, 0x00, 0x00, 0x13);
}
