#ifndef MILIGHTUPDSENDER_H
#define MILIGHTUPDSENDER_H

#include <QtGui>
#include <QUdpSocket>

#include "default_values.h"

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
    void RGBSetBuildinEffect(unsigned char zone);
    void RGBIncreaseSpeed(unsigned char zone);
    void RGBDecreaseSpeed(unsigned char zone);

    void WhiteSetOn(unsigned char zone);
    void WhiteSetOff(unsigned char zone);
    void WhiteSetNight(unsigned char zone);
    void WhiteIncreaseBright(unsigned char zone);
    void WhiteDecreaseBright(unsigned char zone);

private:
    void error(const char *x);
    void udpsend(unsigned char code, unsigned char param);

    QUdpSocket *m_udpSocket;
    QHostAddress m_addr;
    quint16 m_port;

    int currentzone;
};

#endif // MILIGHTUPDSENDER_H
