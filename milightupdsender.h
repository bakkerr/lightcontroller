#ifndef MILIGHTUPDSENDER_H
#define MILIGHTUPDSENDER_H

#include <QtGui>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"

class MiLightUPDsender : public QObject
{
    Q_OBJECT;

public:
    MiLightUPDsender(QObject *parent = 0);

public slots:
    void setColor(unsigned char zone, const QColor &c);
    void setBright(unsigned char zone, unsigned char value);
    void setOn(unsigned char zone);
    void setOff(unsigned char zone);
    void setWhite(unsigned char zone);

private:
    void error(const char *x);
    void udpsend(unsigned char code, unsigned char param);

    int udpSocket;
    struct sockaddr_in destSockAddr;
    int currentzone;
};

#endif // MILIGHTUPDSENDER_H
