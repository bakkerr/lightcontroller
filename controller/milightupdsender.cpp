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

}

void MiLightUPDsender::error(const char* x){
    printf("%s", x);
}

void MiLightUPDsender::udpsend(unsigned char code, unsigned char param)
{
    //qDebug() << "Command: " << code << " Param: " << param << endl;

    const char command[3] = {(char)code, (char)param, (char)0x55};

    for(int i = 0; i < 2; i++){
        qint64 bs = m_udpSocket->writeDatagram(command, 3, m_addr, m_port);
        if(bs <= 0) error("Error sending data!\n");
    }

    this->thread()->usleep(10000);
}

void MiLightUPDsender::RGBSetColor(const QColor &c, unsigned char zone)
{
    int color = (256 + 176 - (int)(c.hue() / 360.0 * 255.0)) % 256;
    RGBSetOn(zone);
    udpsend(0x40, color);
}

void MiLightUPDsender::RGBSetBright(unsigned char value, unsigned char zone)
{
    unsigned char BRIGHTcodes[19] = {0x02, 0x03, 0x04, 0x05, 0x08,
                            0x09, 0x0A, 0x0B, 0x0D, 0x0E,
                            0x0F, 0x10, 0x12, 0x13, 0x14,
                            0x15, 0x17, 0x18, 0x19};

    //qDebug() << "B" << zone << ":" << value << endl;

    if(value < 19)
    {
        RGBSetOn(zone);
        udpsend(0x4E, BRIGHTcodes[value]);
    }
}

void MiLightUPDsender::RGBSetOn(unsigned char zone)
{
    unsigned char ONcodes[5] = {0x42, 0x45, 0x47, 0x49, 0x4B};
    if(zone != currentzone){
        udpsend(ONcodes[zone], 0x00);
        currentzone = zone;
        //this->thread()->usleep(10000);
    }
}

void MiLightUPDsender::RGBSetOff(unsigned char zone)
{
    unsigned char OFFcodes[5] = {0x41, 0x46, 0x48, 0x4A, 0x4C};
    udpsend(OFFcodes[zone], 0x00);
    currentzone = -1;
}

void MiLightUPDsender::RGBSetWhite(unsigned char zone)
{
    unsigned char WHITEcodes[5] = {0xC2, 0xC5, 0xC7, 0xC9, 0xCB};
    udpsend(WHITEcodes[zone], 0x00);
    currentzone = -1;
}

void MiLightUPDsender::RGBSetBuildinEffect(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(0x4D, 0x00);
}

void MiLightUPDsender::RGBIncreaseSpeed(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(0x44, 0x00);
}

void MiLightUPDsender::RGBDecreaseSpeed(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(0x43, 0x00);
}

void MiLightUPDsender::WhiteSetOn(unsigned char zone)
{

}

void MiLightUPDsender::WhiteSetOff(unsigned char zone)
{

}

void MiLightUPDsender::WhiteSetNight(unsigned char zone)
{

}

void MiLightUPDsender::WhiteIncreaseBright(unsigned char zone)
{

}

void MiLightUPDsender::WhiteDecreaseBright(unsigned char zone)
{

}
