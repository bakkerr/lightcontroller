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
    void setColor(const QColor &c, quint16 zone);
    void setBright(unsigned char value, quint16 zone);
    void setOn(quint16 zone);
    void setOff(quint16zone);
    void setWhite(quint16 zone);
    void setBuildinEffect(quint16 zone);
    void increaseSpeed(quint16 zone);
    void decreaseSpeed(quint16 zone);

private:
    void error(const char *x);
    void udpsend(quint8 prefix, quint16 remote, quint8 color, quint8 bright, quint8 key);

    QUdpSocket *m_udpSocket;
    QHostAddress m_addr;
    quint16 m_port;

    quint8 m_seq;


    int currentzone;


};

#endif // MILIGHTUPDSENDER_H
