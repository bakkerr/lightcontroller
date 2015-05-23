#ifndef MILIGHTUPDSENDER_H
#define MILIGHTUPDSENDER_H

#include <QtGui>
#include <QUdpSocket>

#include "default_values.h"



/* Commands arrays  [Command]              [Z0]  [Z1]  [Z2]  [Z3]  [Z4]   */
const unsigned char RGBOnCodes[5]       = {0x42, 0x45, 0x47, 0x49, 0x4B};
const unsigned char RGBOffCodes[5]      = {0x41, 0x46, 0x48, 0x4A, 0x4C};
const unsigned char RGBWhiteCodes[5]    = {0xC2, 0xC5, 0xC7, 0xC9, 0xCB};
const unsigned char RGBNightCodes[5]    = {};
const unsigned char RGBColorCode        =  0x40;
const unsigned char RGBBrightCode       =  0x4E;
const unsigned char RGBEffectCode       =  0x4D;
const unsigned char RGBSpeedDownCode    =  0x43;
const unsigned char RGBSpeedUpCode      =  0x44;
const unsigned char RGBBrightValues[19] = {0x02, 0x03, 0x04, 0x05, 0x08,
                                           0x09, 0x0A, 0x0B, 0x0D, 0x0E,
                                           0x0F, 0x10, 0x12, 0x13, 0x14,
                                           0x15, 0x17, 0x18, 0x19};

const unsigned char whiteOnCodes[5]     = {0x39, 0x38, 0x3D, 0x37, 0x32};
const unsigned char whiteOffCodes[5]    = {0x35, 0x3B, 0x33, 0x3A, 0x36};
const unsigned char whiteNightCodes[5]  = {0xB9, 0xBB, 0xB3, 0xBA, 0xB6};
const unsigned char whiteFullCodes[5]   = {0xB5, 0xB8, 0xBD, 0xB7, 0xB2};
const unsigned char whiteBrightUpCode   =  0x34;
const unsigned char whiteBrightDownCode =  0x3C;
const unsigned char whiteWarmthUpCode   =  0x3E;
const unsigned char whiteWarmthDownCode =  0x3F;




class MiLightUPDsender : public QObject
{
    Q_OBJECT;

public:
    MiLightUPDsender(QString ip, int port = MILIGHT_PORT_DEFAULT, QObject *parent = 0);

public slots:
    void RGBSetColor(const QColor &c, unsigned char zone);
    void RGBSetBright(unsigned char value, unsigned char zone);
    void RGBSetOn(unsigned char zone);
    void RGBSetOff(unsigned char zone);
    void RGBSetWhite(unsigned char zone);
    void RGBSetNight(unsigned char zone);
    void RGBSetBuildinEffect(unsigned char zone);
    void RGBIncreaseSpeed(unsigned char zone);
    void RGBDecreaseSpeed(unsigned char zone);

    void whiteSetOn(unsigned char zone);
    void whiteSetOff(unsigned char zone);
    void whiteSetNight(unsigned char zone);
    void whiteIncreaseBright(unsigned char zone);
    void whiteDecreaseBright(unsigned char zone);
    void whiteIncreaseWarmth(unsigned char zone);
    void whiteDecreaseWarmth(unsigned char zone);

private:
    void error(const char *x);
    void udpsend(unsigned char code, unsigned char param);

    QUdpSocket *m_udpSocket;
    QHostAddress m_addr;
    quint16 m_port;

    int currentzoneRGB;
    int currentzoneW;
};

#endif // MILIGHTUPDSENDER_H
