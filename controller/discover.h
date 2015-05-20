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
#include "settings.h"

class MiLightDiscover : public QDialog
{
    Q_OBJECT;

public:
    MiLightDiscover(QWidget *parent = 0);
    ~MiLightDiscover();

signals:
    void selectedDevices(const QStringList &devices, bool setDefaults);

private slots:
    void discover();
    void userSelected();

private:
    QGroupBox    m_gb;
    QVBoxLayout  m_gbLayout;

    QButtonGroup m_bg;

    QPushButton  m_okButton;
    QPushButton  m_reDiscoverButton;
    QLabel       m_noDevicesFoundLabel;
    QLabel       m_dummyLabel;

    QCheckBox    m_setDefaultCheckBox;

    QStringList UDPdiscover();

};




#endif // DISCOVER_H
