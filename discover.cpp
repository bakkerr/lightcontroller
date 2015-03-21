#include "discover.h"

MiLightDiscover::MiLightDiscover(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *l0 = new QVBoxLayout();

    QGroupBox *gb = new QGroupBox(tr("Select bridges:"));
    QVBoxLayout *l1 = new QVBoxLayout();
    QButtonGroup *bg = new QButtonGroup();
    bg->setExclusive(false);
    QPushButton *ok = new QPushButton(tr("Done"));

    QStringList devices = discover();

    QStringListIterator i(devices);
    while(i.hasNext()){
        QString s = i.next();
        QStringList sl = s.split(',');
        if(sl.size() >= 2){
            QCheckBox *j = new QCheckBox(s);
            j->setChecked(true);
            bg->addButton(j);
            l1->addWidget(j);
        }
    }

    gb->setLayout(l1);

    l0->addWidget(gb);
    l0->addWidget(ok);

    connect(ok, SIGNAL(clicked()), this, SIGNAL(userSelected()));

    setLayout(l0);
}

MiLightDiscover::~MiLightDiscover()
{

}

QStringList MiLightDiscover::discover()
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
            //if(!sl.contains(s)){
                qDebug() << s << endl;
                sl.append(s);
            //}
        }
    }

    return sl;

}
