#include "milightupdsender.h"

MiLightUPDsender::MiLightUPDsender(QObject *parent, QString ip, int port) :
    QObject(parent)
{
    QByteArray ba = ip.toLocal8Bit();
    char *c_ip = ba.data();

    qDebug() << port << ip << endl;

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) error("Couldn't open socket\n");

    memset(&destSockAddr, 0, sizeof(destSockAddr));

    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_port = htons(port);
    destSockAddr.sin_addr.s_addr = inet_addr(c_ip);

    currentzone = -1;

}

void MiLightUPDsender::error(const char* x){
    printf("%s", x);
}

void MiLightUPDsender::udpsend(unsigned char code, unsigned char param)
{
    unsigned char command[3] = {code, param, 0x55};
    int bs = sendto(udpSocket, command, 3, 0, (const struct sockaddr*)&destSockAddr, sizeof(destSockAddr));
    if(bs <= 0) error("Error sending data!\n");
    bs = sendto(udpSocket, command, 3, 0, (const struct sockaddr*)&destSockAddr, sizeof(destSockAddr));
    if(bs <= 0) error("Error sending data!\n");
    usleep(10000);

}

void MiLightUPDsender::setColor(unsigned char zone, const QColor &c)
{
    int color = (256 + 176 - (int)(c.hue() / 360.0 * 255.0)) % 256;
    setOn(zone);
    udpsend(0x40, color);
}

void MiLightUPDsender::setBright(unsigned char zone, unsigned char value)
{
    unsigned char BRIGHTcodes[19] = {0x02, 0x03, 0x04, 0x05, 0x08,
                            0x09, 0x0A, 0x0B, 0x0D, 0x0E,
                            0x0F, 0x10, 0x12, 0x13, 0x14,
                            0x15, 0x17, 0x18, 0x19};

    if(value < 19)
    {
        setOn(zone);
        udpsend(0x4E, BRIGHTcodes[value]);
    }
}

void MiLightUPDsender::setOn(unsigned char zone)
{
    unsigned char ONcodes[5] = {0x42, 0x45, 0x47, 0x49, 0x4B};
    if(zone != currentzone){
      udpsend(ONcodes[zone], 0x00);
      currentzone = zone;
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
    unsigned char WHITEcodes[5] = {0xC2, 0xC5, 0xC7, 0xC9, 0xCB};
    udpsend(WHITEcodes[zone], 0x00);
    currentzone = -1;
}
