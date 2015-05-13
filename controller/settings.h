#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QSettings>

#include "default_values.h"

extern bool GLOBAL_settingsChanged;

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = 0);
    ~settingsDialog();

signals:

public slots:
    void displaySettings(const QString setting = tr("default"));

private:
    QTreeWidgetItem *__newNesting(QSettings *s, QTreeWidgetItem *twi);
    QGroupBox *m_preferencesBox;
    QLabel *m_udpSleepLabel;
    QLineEdit *m_udpSleepText;
    QPushButton *m_saveButton;

    QGroupBox *m_settingsManager;
    QTreeWidget *m_settingsDisplay;


};

#endif // SETTINGS_H
