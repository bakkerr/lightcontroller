#ifndef PRESETCONTROLLER_H
#define PRESETCONTROLLER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDockWidget>
#include <QAbstractTableModel>
#include <QTableWidget>

#include <QMessageBox>

#include "default_values.h"
#include "settings.h"
#include "preset.h"

class PresetController : public QDockWidget
{
    Q_OBJECT

public:
    PresetController(QWidget *parent = 0);

    void loadSettings(QSettings *s);
    void saveSettings(QSettings *s);

signals:
    void createPreset();
    void setPreset(Preset *p);

private slots:
    void addPreset(Preset *p);
    void cellChanged(int row, int column);
    void cellClicked(int row, int column);

private:
    int m_instanceNum;
    QTableWidget *m_tw;

    QList<Preset*> m_pList;

    QPushButton *m_createPreset;

};

#endif // PRESETCONTROLLER_H
