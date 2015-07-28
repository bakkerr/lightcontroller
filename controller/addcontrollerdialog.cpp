#include "addcontrollerdialog.h"

addControllerDialog::addControllerDialog(QList<SingleController *> controllers, QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Add Controller"));
    m_mainLayout = new QGridLayout();

    QLabel *nameLabel = new QLabel(tr("Name:"), this);
    m_nameLineEdit = new QLineEdit(this);

    m_hasRemoteGroupBox = new QGroupBox(tr("Remote:"), this);
    m_hasRemoteGroupBox->setCheckable(true);
    m_hasRemoteGroupBox->setChecked(true);
    QHBoxLayout *rl = new QHBoxLayout();
    m_remoteLineEdit = new QLineEdit(this);
    rl->addWidget(m_remoteLineEdit);
    m_hasRemoteGroupBox->setLayout(rl);

    m_slaveGroupBox = new QGroupBox(tr("This controller is a Master for:"), this);
    m_slaveSelect = new QButtonGroup(this);
    m_slaveSelect->setExclusive(false);

    QVBoxLayout *sl = new QVBoxLayout();

    m_nextID = 0;
    quint16 nextRemote = 0;
    foreach(SingleController *c, controllers){
        m_existingRemotes.append(c->remote());

        if(c->id() > m_nextID){
            m_nextID = c->id();
        }

        if(c->remote() > nextRemote){
            nextRemote = c->remote();
        }
        QString remoteString = QString::number(c->remote(), 16);
        while(remoteString.length() < 4){
            remoteString.prepend("0");
        }

        QCheckBox *cb = new QCheckBox(c->name() + tr(" (") + remoteString + tr(")"), this);
        sl->addWidget(cb);
        m_slaveSelect->addButton(cb, c->id());
    }

    nextRemote++;
    QString remoteString = QString::number(nextRemote, 16);
    while(remoteString.length() < 4){
        remoteString.prepend("0");
    }
    m_remoteLineEdit->setText(remoteString);

    m_slaveGroupBox->setLayout(sl);

    m_okButton = new QPushButton(tr("&Add"), this);
    m_cancelButton = new QPushButton(tr("&Cancel"), this);

    connect(m_okButton, SIGNAL(clicked()), this, SLOT(checkValues()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    m_mainLayout->addWidget(nameLabel, 1, 1, 1, 1);
    m_mainLayout->addWidget(m_nameLineEdit, 1, 2, 1, 2);
    m_mainLayout->addWidget(m_hasRemoteGroupBox, 2, 1, 1, 3);
    m_mainLayout->addWidget(m_slaveGroupBox, 3, 1, 1, 3);
    m_mainLayout->addWidget(m_okButton, 4, 2, 1, 1);
    m_mainLayout->addWidget(m_cancelButton, 4, 3, 1, 1);

    m_nextID++;

    setLayout(m_mainLayout);
}

void addControllerDialog::checkValues()
{
    QString remoteValue = m_remoteLineEdit->text().toLatin1();
    bool ok;
    quint16 remote = remoteValue.toUInt(&ok, 16);

    if(m_nameLineEdit->text().isEmpty()){
        QMessageBox::warning(this, tr("Error!"), tr("Name cannot be empty!"));
        return;
    }

    if(m_hasRemoteGroupBox->isChecked()){
        if((remoteValue.length() < 1 || remoteValue.length() > 4)){
            QMessageBox::warning(this, tr("Error!"), tr("Invalid RemoteID - Should have length 1 to 4"));
            return;
        }

        if((!ok || remote == 0)){
            QMessageBox::warning(this, tr("Error!"), tr("Invalid RemoteID - Invalid (or zero) Hex string [0-9, A-F]"));
            return;
        }

        if(m_existingRemotes.contains(remote)){
            QMessageBox::warning(this, tr("Error!"), tr("Remote ID already in use!"));
            return;
        }
    }

    QList<quint16> selectedSlaves;
    foreach(QAbstractButton *b, m_slaveSelect->buttons()){
        if(b->isChecked()){
            selectedSlaves.append(m_slaveSelect->id(b));
        }
    }

    if(!m_hasRemoteGroupBox->isChecked()){
        if(selectedSlaves.size() == 0){
            QMessageBox::warning(this, tr("Error!"), tr("Only mastercontrollers can have no remote!"));
            return;
         }
         remote = 0;
    }

    emit(addController(m_nextID, m_nameLineEdit->text(), remote, selectedSlaves));

    close();
}
