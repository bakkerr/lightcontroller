#ifndef MILIGHTUPDSENDER_H
#define MILIGHTUPDSENDER_H

#include <QtGui>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "default_values.h"

class MiLightUPDsender : public QObject
{
    Q_OBJECT;

public:
    MiLightUPDsender(QString ip, int port = MILIGHT_PORT_DEFAULT, QObject *parent = 0);

public slots:
    void setColor(const QColor &c, unsigned char zone);
    void setBright(unsigned char value, unsigned char zone);
    void setOn(unsigned char zone);
    void setOff(unsigned char zone);
    void setWhite(unsigned char zone);
    void setBuildinEffect(unsigned char zone);
    void increaseSpeed(unsigned char zone);
    void decreaseSpeed(unsigned char zone);

private:
    void error(const char *x);
    void udpsend(unsigned char code, unsigned char param);

    int udpSocket;
    struct sockaddr_in destSockAddr;
    int currentzone;
};

#endif // MILIGHTUPDSENDER_H
