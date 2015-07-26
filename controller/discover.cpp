#include "discover.h"

MiLightDiscover::MiLightDiscover(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("WiFi Bridge Detection"));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    m_devicesGroupbox.setTitle(tr("Found Bridges:"));
    m_devicesGroupbox.setVisible(false);
    m_devicesGroupbox.setLayout(&m_devicesGroupboxLayout);

    m_devicesButtonGroup.setExclusive(false);

    m_settingsGroupbox.setTitle(tr("Load settings:"));
    m_settingsGroupbox.setCheckable(true);
    m_settingsGroupbox.setLayout(&m_settingsGroupboxLayout);
    m_settingsButtonGroup.setExclusive(true);

    m_okButton.setText(tr("Done"));
    m_reDiscoverButton.setText(tr("Re-discover"));
    m_noDevicesFoundLabel.setText(tr("No bridges found :("));
    m_noDevicesFoundLabel.setAlignment(Qt::AlignCenter);
    m_noDevicesFoundLabel.setVisible(false);
    m_dummyLabel.setText(tr("\nDummy devices"));

    m_setDefaultCheckBox.setText(tr("Enable all zones, Blue, max brightness"));
    m_setDefaultCheckBox.setChecked(true);

    getSettings();

    mainLayout->addWidget(&m_reDiscoverButton);
    mainLayout->addWidget(&m_noDevicesFoundLabel);
    mainLayout->addWidget(&m_devicesGroupbox);
    mainLayout->addWidget(&m_settingsGroupbox);
    mainLayout->addWidget(&m_setDefaultCheckBox);
    mainLayout->addWidget(&m_okButton);

    connect(&m_devicesButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(checkSettings()));

    connect(&m_reDiscoverButton, SIGNAL(clicked()), this, SLOT(discover()));
    connect(&m_okButton, SIGNAL(clicked()), this, SLOT(discoveryDone()));

    setLayout(mainLayout);

    m_okButton.setFocus();

    discover();

}

MiLightDiscover::~MiLightDiscover()
{

}

QStringList MiLightDiscover::getSelectedDevices()
{
    QStringList selection;
    QList<QAbstractButton*> x = m_devicesButtonGroup.buttons();
    for(int i = 0; i < x.size(); i++){
        if(x.at(i)->isChecked()){
            selection.append(x.at(i)->text());
        }
    }

    return selection;


}

void MiLightDiscover::discoveryDone()
{
    QStringList selection = getSelectedDevices();
    if(selection.size() > 0){
        emit selectedDevices(selection, m_setDefaultCheckBox.checkState());
        done(0);
    }
}

void MiLightDiscover::discover()
{
    setEnabled(false);
    repaint();
    m_devicesGroupboxLayout.removeWidget(&m_dummyLabel);
    QList<QAbstractButton *> b = m_devicesButtonGroup.buttons();
    for(int i = 0; i < b.size(); i++){
        m_devicesButtonGroup.removeButton(b.at(i));
        delete b.at(i);
    }

    QStringList devices = UDPdiscover();

    if(devices.length() == 0){
        m_noDevicesFoundLabel.setVisible(true);
        m_devicesGroupbox.setVisible(false);
    }
    else{
        m_noDevicesFoundLabel.setVisible(false);
    }

    if(devices.length() > 0){
        m_devicesGroupbox.setVisible(true);
        QStringListIterator i(devices);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s);
                j->setChecked(true);
                m_devicesButtonGroup.addButton(j);
                m_devicesGroupboxLayout.addWidget(j);
            }
        }
    }

#ifdef ADD_DUMMY_DEVICES
    m_devicesGroupboxLayout.addWidget(&m_dummyLabel);
    QStringList dummys;
    dummys.append(QString("1.2.3.4,DUMMY01DUMMY,"));
    dummys.append(QString("4.3.2.1,DUMMY02DUMMY,"));
    if(dummys.length() > 0){
        m_devicesGroupbox.setVisible(true);
        QStringListIterator i(dummys);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s, this);
                j->setChecked(false);
                m_devicesButtonGroup.addButton(j);
                m_devicesGroupboxLayout.addWidget(j);
            }
        }
    }
#endif

    checkSettings();
    setEnabled(true);
}

void MiLightDiscover::getSettings()
{
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);

    QStringList settings = s->childGroups();
    for(int i = 0; i < settings.size(); i++){
        QRadioButton *b = new QRadioButton(settings.at(i), this);
        m_settingsButtonGroup.addButton(b);
        m_settingsGroupboxLayout.addWidget(b);
    }
}

void MiLightDiscover::checkSettings()
{
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
    int numValid = 0;

    QStringList devicesChecked;
    QStringList selectedDevices = getSelectedDevices();
    for(int i = 0; i < selectedDevices.size(); i++){
        QStringList sl = selectedDevices.at(i).split(',');
        if(sl.size() >= 2){
            devicesChecked.append(sl.at(1));
        }
    }

    devicesChecked.sort();

    QList<QAbstractButton *> availableSettings = m_settingsButtonGroup.buttons();

    for(int i = 0; i < availableSettings.size(); i++){
        QAbstractButton *b = availableSettings.at(i);

        QStringList devicesInSetting;
        s->beginGroup(b->text());
        int size = s->beginReadArray(tr("LightControllers"));
        for(int j = 0; j < size; j++){
            s->setArrayIndex(j);
            devicesInSetting.append(s->value(tr("id"), tr("")).toString());
        }

        s->endArray();
        s->endGroup();

        devicesInSetting.sort();

        if(devicesInSetting == devicesChecked){
            b->setEnabled(true);
            if(numValid == 0 || b->text() == QString(DEFAULT_SAVE_NAME)){
                b->setChecked(true);
            }
            numValid++;
        }
        else{
            b->setEnabled(false);
        }
    }

    if(numValid > 0){
        m_settingsGroupbox.setChecked(true);
    }
    else{
        m_settingsGroupbox.setChecked(false);
    }
}

QStringList MiLightDiscover::UDPdiscover()
{
    QStringList sl;

    QUdpSocket *os = new QUdpSocket();
    os->bind(MILIGHT_PORT_DISCOVER, QUdpSocket::ShareAddress);
    QByteArray msg = MILIGHT_MSG_DISCOVER;

    for(int i = 0; i < 5; i++){
        os->writeDatagram(msg.data(), msg.size(), QHostAddress::Broadcast, 48899);
        usleep(50000);
    }

    usleep(500000);

    while(os->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(os->pendingDatagramSize());
        os->readDatagram(datagram.data(), datagram.size());

        if(datagram != msg){
            QString s = QString(datagram);
            if(!sl.contains(s)){
                sl.append(s);
            }
        }
    }

    delete os;

    return sl;

}

void MiLightDiscover::closeEvent(QCloseEvent *event)
{
    exit(EXIT_SUCCESS);
}
