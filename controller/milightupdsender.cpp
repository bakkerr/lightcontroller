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

    currentzoneRGB = -1;
    currentzoneW = -1;

}

void MiLightUPDsender::error(const char* x){
    printf("%s", x);
}

void MiLightUPDsender::udpsend(unsigned char code, unsigned char param)
{
    const char command[3] = {(char)code, (char)param, (char)0x55};

    //qDebug() << "Command: " << code << " Param: " << param << endl;

    for(int i = 0; i < 2; i++){
        qint64 bs = m_udpSocket->writeDatagram(command, 3, m_addr, m_port);
        if(bs <= 0) error("Error sending data!\n");
    }

    this->thread()->usleep(10000);
}

void MiLightUPDsender::RGBSetOn(unsigned char zone)
{
    if(zone != currentzoneRGB){
        udpsend(RGBOnCodes[zone], 0x00);
        currentzoneRGB = zone;
    }
}

void MiLightUPDsender::RGBSetOff(unsigned char zone)
{
    udpsend(RGBOffCodes[zone], 0x00);
    currentzoneRGB = -1;
}

void MiLightUPDsender::RGBSetWhite(unsigned char zone)
{
    udpsend(RGBWhiteCodes[zone], 0x00);
    currentzoneRGB = -1;
}

void MiLightUPDsender::RGBSetNight(unsigned char zone)
{
    RGBSetOff(zone);
    udpsend(RGBNightCodes[zone], 0x00);
    currentzoneRGB = -1;
}

void MiLightUPDsender::RGBSetColor(const QColor &c, unsigned char zone)
{
    int color = (256 + 176 - (int)(c.hue() / 360.0 * 255.0)) % 256;
    RGBSetOn(zone);
    udpsend(RGBColorCode, color);
}

void MiLightUPDsender::RGBSetBright(unsigned char value, unsigned char zone)
{
    if(value < 19)
    {
        RGBSetOn(zone);
        udpsend(RGBBrightCode, RGBBrightValues[value]);
    }
}

void MiLightUPDsender::RGBSetBuildinEffect(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(RGBEffectCode, 0x00);
}

void MiLightUPDsender::RGBIncreaseSpeed(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(RGBSpeedUpCode, 0x00);
}

void MiLightUPDsender::RGBDecreaseSpeed(unsigned char zone)
{
    RGBSetOn(zone);
    udpsend(RGBSpeedDownCode, 0x00);
}

void MiLightUPDsender::whiteSetOn(unsigned char zone)
{
    if(zone != currentzoneW){
        udpsend(whiteOnCodes[zone], 0x00);
        currentzoneW = zone;
    }
}

void MiLightUPDsender::whiteSetOff(unsigned char zone)
{
    udpsend(whiteOffCodes[zone], 0x00);
    currentzoneW = -1;
}

void MiLightUPDsender::whiteSetNight(unsigned char zone)
{
    whiteSetOff(zone);
    udpsend(whiteNightCodes[zone], 0x00);
    currentzoneW = -1;
}

void MiLightUPDsender::whiteIncreaseBright(unsigned char zone)
{
    whiteSetOn(zone);
    udpsend(whiteBrightUpCode, 0x00);
}

void MiLightUPDsender::whiteDecreaseBright(unsigned char zone)
{
    whiteSetOn(zone);
    udpsend(whiteBrightDownCode, 0x00);
}

void MiLightUPDsender::whiteIncreaseWarmth(unsigned char zone)
{
    whiteSetOn(zone);
    udpsend(whiteWarmthUpCode, 0x00);
}

void MiLightUPDsender::whiteDecreaseWarmth(unsigned char zone)
{
    whiteSetOn(zone);
    udpsend(whiteWarmthDownCode, 0x00);
}
