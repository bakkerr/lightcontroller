#ifndef MILIGHTUPDSENDER_H
#define MILIGHTUPDSENDER_H

#include <QtGui>
#include <QUdpSocket>

#include "default_values.h"

class MiLightUPDsender : public QObject
{
    Q_OBJECT;

public:
    MiLightUPDsender(QString ip, int port = UDP_PORT_DEFAULT, QObject *parent = 0);

public slots:
    void setColor(const QColor &c, quint16 zone);
    void setBright(unsigned char value, quint16 zone);
    void setOn(quint16 zone);
    void setOff(quint16 zone);
    void setWhite(quint16 zone);
    void setNight(quint16 zone);
    void setBuildinEffect(quint8 effect, quint16 zone);
    void increaseSpeed(quint16 zone, quint8 effect);
    void decreaseSpeed(quint16 zone, quint8 effect);
    void pair(quint16 zone);
    void unPair(quint16 zone);

private:
    void error(const char *x);
    void udpsend(quint16 remote, quint8 color, quint8 bright, quint8 key, quint8 prefix = 0xB8);

    QUdpSocket *m_udpSocket;
    QHostAddress m_addr;
    quint16 m_port;

    quint8 m_udpResends;
    quint8 m_resends;

};

#endif // MILIGHTUPDSENDER_H

