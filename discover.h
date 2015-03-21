#ifndef DISCOVER_H
#define DISCOVER_H

#include <QtGui>
#include <QUdpSocket>
#include <QDebug>

#include <unistd.h>

class MiLightDiscover : public QWidget
{
    Q_OBJECT;

public:
    MiLightDiscover(QWidget *parent = 0);
    ~MiLightDiscover();

signals:
    void userSelected();

public slots:
    QStringList discover();

};




#endif // DISCOVER_H
