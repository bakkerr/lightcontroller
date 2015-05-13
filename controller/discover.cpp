#include "discover.h"

MiLightDiscover::MiLightDiscover(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("WiFi Bridge Detection"));

    QVBoxLayout *mainLayout = new QVBoxLayout();

    m_gb.setTitle(tr("Found Bridges:"));
    m_gb.setVisible(false);
    m_gb.setLayout(&m_gbLayout);

    m_bg.setExclusive(false);

    m_okButton.setText(tr("Done"));
    m_reDiscoverButton.setText(tr("Re-discover"));
    m_noDevicesFoundLabel.setText(tr("No bridges found :("));
    m_noDevicesFoundLabel.setAlignment(Qt::AlignCenter);
    m_noDevicesFoundLabel.setVisible(false);

    m_setDefaultCheckBox.setText(tr("Enable all zones, Blue, max brightness"));
    m_setDefaultCheckBox.setChecked(true);

    mainLayout->addWidget(&m_reDiscoverButton);
    mainLayout->addWidget(&m_noDevicesFoundLabel);
    mainLayout->addWidget(&m_gb);
    mainLayout->addWidget(&m_setDefaultCheckBox);
    mainLayout->addWidget(&m_okButton);

    connect(&m_reDiscoverButton, SIGNAL(clicked()), this, SLOT(discover()));
    connect(&m_okButton, SIGNAL(clicked()), this, SLOT(userSelected()));

    setLayout(mainLayout);

    m_okButton.setFocus();

    discover();

}

MiLightDiscover::~MiLightDiscover()
{
    QList<QAbstractButton *> b = m_bg.buttons();
    for(int i = 0; i < b.size(); i++){
        m_bg.removeButton(b.at(i));
        delete b.at(i);
    }
}

void MiLightDiscover::userSelected()
{

    QStringList selection;
    QList<QAbstractButton*> x = m_bg.buttons();
    for(int i = 0; i < x.size(); i++){
        if(x.at(i)->isChecked()){
            selection.append(x.at(i)->text());
        }
    }

    if(selection.size() > 0){
        emit selectedDevices(selection, m_setDefaultCheckBox.checkState());
        done(0);
    }
}

void MiLightDiscover::discover()
{
    QList<QAbstractButton *> b = m_bg.buttons();
    for(int i = 0; i < b.size(); i++){
        m_bg.removeButton(b.at(i));
        delete b.at(i);
    }

    QStringList devices = UDPdiscover();

    if(devices.length() == 0){
        m_noDevicesFoundLabel.setVisible(true);
        m_gb.setVisible(false);
    }
    else{
        m_noDevicesFoundLabel.setVisible(false);
    }

    if(devices.length() > 0){
        m_gb.setVisible(true);
        QStringListIterator i(devices);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s);
                j->setChecked(true);
                m_bg.addButton(j);
                m_gbLayout.addWidget(j);
            }
        }
    }

#ifdef ADD_DUMMY_DEVICES
    m_gbLayout.addSpacing(25);
    m_gbLayout.addWidget(new QLabel(tr("Dummy devices:")));
    QStringList dummys;
    dummys.append(QString("1.2.3.4,DUMMY01DUMMY,"));
    dummys.append(QString("4.3.2.1,DUMMY02DUMMY,"));
    if(dummys.length() > 0){
        m_gb.setVisible(true);
        QStringListIterator i(dummys);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s);
                j->setChecked(false);
                m_bg.addButton(j);
                m_gbLayout.addWidget(j);
            }
        }
    }
#endif

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

    usleep(250000);

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
