#ifndef DISCOVER_H
#define DISCOVER_H

#include <QtGui>
#include <QUdpSocket>
#include <QDebug>


QList<QString> discover()
{
    QList<QString> sl;

    QUdpSocket *os = new QUdpSocket();
    os->bind(48899, QUdpSocket::ShareAddress);
    QByteArray msg = "Link_Wi-Fi";

    for(int i = 0; i < 5; i++){
        os->writeDatagram(msg.data(), msg.size(), QHostAddress::Broadcast, 48899);
        usleep(50000);
    }

    usleep(250000);

    while(os->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(os->pendingDatagramSize());
        os->readDatagram(datagram.data(), datagram.size());

        if(datagram != msg){
            QString s = QString(datagram);
            if(!sl.contains(s)) sl.append(s);
        }
    }

    return sl;

/*
    int udpSocket;
    int bc = 1;
    struct sockaddr_in destSockAddr;

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) printf("Couldn't open socket\n");
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &bc, sizeof(bc));

    memset(&destSockAddr, 0, sizeof(destSockAddr));

    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_port = htons(48899);
    destSockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    char b[] = "Link_Wi-Fi";

    for(int i = 0; i < 20; i++){
        int bs = sendto(udpSocket, b, 11, 0, (const struct sockaddr*)&destSockAddr, sizeof(destSockAddr));
        if(bs <= 0) printf("Error sending %d data! (%d) \n", i, errno);
        usleep(50000);
    }

    const char* hostname=0;
    const char* portname="daytime";
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_protocol=0;
    hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
    struct addrinfo* res=0;
    int err=getaddrinfo(hostname,portname,&hints,&res);
    if (err!=0) {
        printf("failed to resolve local socket address (err=%d)",err);
    }
*/

}

#endif // DISCOVER_H
