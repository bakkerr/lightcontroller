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

#include <QButtonGroup>

#include <QMessageBox>

#include "default_values.h"
#include "singlecontroller.h"

class addControllerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit addControllerDialog(QList<SingleController*> controllers, QWidget *parent = 0);

signals:
    void addController(QString Name, quint16 remote, QList<quint16> slaves);

public slots:

private slots:
    void checkValues();

private:
    QList<quint16> m_existingRemotes;

    QGridLayout *m_mainLayout;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_remoteLineEdit;

    QGroupBox *m_hasRemoteGroupBox;

    QGroupBox *m_slaveGroupBox;
    QButtonGroup *m_slaveSelect;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
};

#endif // ADDCONTROLLERDIALOG_H
