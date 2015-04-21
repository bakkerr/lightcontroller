#ifndef DISCOVER_H
#define DISCOVER_H

#include <QDialog>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>

#include <QUdpSocket>
#include <QDebug>

#include <unistd.h>

#include "default_values.h"

class MiLightDiscover : public QDialog
{
    Q_OBJECT;

public:
    MiLightDiscover(QWidget *parent = 0);
    ~MiLightDiscover();

signals:
    void selectedDevices(const QStringList &devices);

private slots:
    void discover();
    void userSelected();

private:
    QVBoxLayout  mainLayout;
    QGroupBox    gb;
    QVBoxLayout  gbLayout;

    QButtonGroup bg;

    QPushButton  ok;
    QPushButton  rd;
    QLabel       noDevicesFound;

    QStringList UDPdiscover();

};




#endif // DISCOVER_H
