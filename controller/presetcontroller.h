#ifndef PRESETCONTROLLER_H
#define PRESETCONTROLLER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDockWidget>
#include <QAbstractTableModel>
#include <QTableView>

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
    void dataChanged(const QModelIndex &tl, const QModelIndex &br);
    void cellClicked(const QModelIndex mi);

private:
    int m_instanceNum;
    QStandardItemModel *m_presetItemModel;
    QTableView *m_presetTableView;

    QList<Preset*> m_presetList;

    QPushButton *m_createPresetButton;

};

#endif // PRESETCONTROLLER_H
