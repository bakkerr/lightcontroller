#ifndef DISCOVER_H
#define DISCOVER_H

#include <QtGui>
#include <QUdpSocket>
#include <QDebug>

#include <unistd.h>

#include "default_values.h"

class MiLightDiscover : public QWidget
{
    Q_OBJECT;

public:
    MiLightDiscover(QWidget *parent = 0);
    ~MiLightDiscover();
    bool done;

signals:
    void userSelected();

public slots:
    QStringList discover();

};




#endif // DISCOVER_H
