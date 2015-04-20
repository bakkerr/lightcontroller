#include "discover.h"

MiLightDiscover::MiLightDiscover(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr("WiFi Bridge Detection"));

    gb.setTitle(tr("Found Bridges:"));
    gb.setVisible(false);
    gb.setLayout(&gbLayout);

    bg.setExclusive(false);

    ok.setText(tr("Done"));
    rd.setText(tr("Re-discover"));
    noDevicesFound.setText(tr("No bridges found :("));
    noDevicesFound.setAlignment(Qt::AlignCenter);
    noDevicesFound.setVisible(false);

    mainLayout.addWidget(&rd);
    mainLayout.addWidget(&noDevicesFound);
    mainLayout.addWidget(&gb);
    mainLayout.addWidget(&ok);

    connect(&rd, SIGNAL(clicked()), this, SLOT(discover()));
    connect(&ok, SIGNAL(clicked()), this, SLOT(userSelected()));

    setLayout(&mainLayout);

    ok.setFocus();

    discover();

}

MiLightDiscover::~MiLightDiscover()
{
    QList<QAbstractButton *> b = bg.buttons();
    for(int i = 0; i < b.size(); i++){
        bg.removeButton(b.at(i));
        delete b.at(i);
    }
}

void MiLightDiscover::userSelected()
{

    QStringList selection;
    QList<QAbstractButton*> x = bg.buttons();
    for(int i = 0; i < x.size(); i++){
        if(x.at(i)->isChecked()){
            selection.append(x.at(i)->text());
        }
    }

    if(selection.size() > 0){
        emit selectedDevices(selection);
        done(0);
    }
}

void MiLightDiscover::discover()
{
    QList<QAbstractButton *> b = bg.buttons();
    for(int i = 0; i < b.size(); i++){
        bg.removeButton(b.at(i));
        delete b.at(i);
    }

    QStringList devices = UDPdiscover();

    if(devices.length() == 0){
        noDevicesFound.setVisible(true);
        gb.setVisible(false);
    }
    else{
        noDevicesFound.setVisible(false);
    }

    if(devices.length() > 0){
        gb.setVisible(true);
        QStringListIterator i(devices);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s);
                j->setChecked(true);
                bg.addButton(j);
                gbLayout.addWidget(j);
            }
        }
    }

#ifdef ADD_DUMMY_DEVICES
    gbLayout.addSpacing(25);
    gbLayout.addWidget(new QLabel(tr("Dummy devices:")));
    QStringList dummys;
    dummys.append(QString("1.2.3.4,DUMMY01DUMMY,"));
    dummys.append(QString("4.3.2.1,DUMMY02DUMMY,"));
    if(dummys.length() > 0){
        gb.setVisible(true);
        QStringListIterator i(dummys);
        while(i.hasNext()){
            QString s = i.next();
            QStringList sl = s.split(',');
            if(sl.size() >= 2){
                QCheckBox *j = new QCheckBox(s);
                j->setChecked(false);
                bg.addButton(j);
                gbLayout.addWidget(j);
            }
        }
    }
#endif

}

QStringList MiLightDiscover::UDPdiscover()
{
    QStringList sl;

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
            if(!sl.contains(s)){
                sl.append(s);
            }
        }
    }

    delete os;

    return sl;

}
