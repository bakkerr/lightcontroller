#ifndef ADDCONTROLLERDIALOG_H
#define ADDCONTROLLERDIALOG_H

#include <QObject>
#include <QDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QButtonGroup>

#include <QMessageBox>

#include "default_values.h"
#include "singlecontroller.h"
#include "container.h"

class addControllerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit addControllerDialog(QList<SingleController*> controllers, QList<container*> containers, QWidget *parent = 0);

signals:
    void addController(quint16 id, QString name, quint16 remote, QList<quint16> slaves, quint16 containerID);

public slots:

private slots:
    void checkValues();

private:
    quint16 m_nextID;
    QList<quint16> m_existingRemotes;

    QGridLayout *m_mainLayout;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_remoteLineEdit;

    QComboBox *m_containerBox;

    QGroupBox *m_hasRemoteGroupBox;

    QGroupBox *m_slaveGroupBox;
    QButtonGroup *m_slaveSelect;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // ADDCONTROLLERDIALOG_H
