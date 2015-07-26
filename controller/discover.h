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
#include <QRadioButton>
#include <QButtonGroup>

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
    void closeEvent(QCloseEvent *event);
    void getSettings();
    void checkSettings();
    QStringList getSelectedDevices();
    void discoveryDone();
    void discover();


private:
    QGroupBox    m_devicesGroupbox;
    QVBoxLayout  m_devicesGroupboxLayout;
    QButtonGroup m_devicesButtonGroup;

    QGroupBox    m_settingsGroupbox;
    QVBoxLayout  m_settingsGroupboxLayout;
    QButtonGroup m_settingsButtonGroup;

    QPushButton  m_okButton;
    QPushButton  m_reDiscoverButton;
    QLabel       m_noDevicesFoundLabel;
    QLabel       m_dummyLabel;

    QCheckBox    m_setDefaultCheckBox;


    QStringList UDPdiscover();

};




#endif // DISCOVER_H
