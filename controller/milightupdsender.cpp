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

    currentzone = -1;

    m_seq = 0;

}

void MiLightUPDsender::error(const char* x){
    printf("%s\n", x);
}

void MiLightUPDsender::udpsend(quint8 prefix, quint16 remote, quint8 color, quint8 bright, quint8 key)
{
    //qDebug() << "Command: " << code << " Param: " << param << endl;

    const char command[7] = {(char)prefix, (char)(remote >> 8), (char)(remote & 0xFF),
                             (char)color,  (char)bright, (char)key, (char)m_seq}

    for(int i = 0; i < 1; i++){
        qint64 bs = m_udpSocket->writeDatagram(command, 3, m_addr, m_port);
        if(bs <= 0) error("Error sending data!\n");
    }

    m_seq++;
    //this->thread()->usleep(10000);
}

void MiLightUPDsender::setColor(const QColor &c, quint16 zone)
{
    int color = (256 + 176 - (int)(c.hue() / 360.0 * 255.0)) % 256;
    udpsend(0x40, color);
}

void MiLightUPDsender::setBright(unsigned char value, quint16 zone)
{
    unsigned char BRIGHTcodes[19] = {0x02, 0x03, 0x04, 0x05, 0x08,
                            0x09, 0x0A, 0x0B, 0x0D, 0x0E,
                            0x0F, 0x10, 0x12, 0x13, 0x14,
                            0x15, 0x17, 0x18, 0x19};

    //qDebug() << "B" << zone << ":" << value << endl;

    if(value < 19)
    {
        udpsend(0xB8, zone, 0x00, BRIGHTcodes[value], 0x0E);
    }
}

void MiLightUPDsender::setOn(unsigned char zone)
{
    unsigned char ONcodes[5] = {0x42, 0x45, 0x47, 0x49, 0x4B};
    if(zone != currentzone){
        udpsend(ONcodes[zone], 0x00);
        currentzone = zone;
        //this->thread()->usleep(10000);
    }
}

void MiLightUPDsender::setOff(unsigned char zone)
{
    unsigned char OFFcodes[5] = {0x41, 0x46, 0x48, 0x4A, 0x4C};
    udpsend(OFFcodes[zone], 0x00);
    currentzone = -1;
}

void MiLightUPDsender::setWhite(unsigned char zone)
{
    udpsend(0xB8, zone, 0x00, 0x00, 0x11);
    currentzone = -1;
}

void MiLightUPDsender::setBuildinEffect(unsigned char zone)
{
    setOn(zone);
    udpsend(0x4D, 0x00);
}

void MiLightUPDsender::increaseSpeed(unsigned char zone)
{
    setOn(zone);
    udpsend(0x44, 0x00);
}

void MiLightUPDsender::decreaseSpeed(unsigned char zone)
{
    setOn(zone);
    udpsend(0x43, 0x00);
}
