#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QDockWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QPushButton>
#include <QSpinBox>
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
    void displaySettings();

private:
    QTreeWidgetItem *__newNesting(QSettings *s, QTreeWidgetItem *twi);

    QGroupBox *m_settingsManager;
    QTreeWidget *m_settingsDisplay;
};

class settingsWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit settingsWidget(QWidget *parent = 0);

signals:
    void udpResends(int);
    void wirelessResends(int);

private:
    QGroupBox *m_preferencesBox;
    QSpinBox *m_udpRepeat;
    QSpinBox *m_wirelessRepeat;
};

#endif // SETTINGS_H
