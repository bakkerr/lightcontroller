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

#include "preset.h"

/*class PresetModel : public QStandardItemModel
{
    Q_OBJECT

public:
    PresetModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    int m_numRows;


};*/

class PresetController : public QDockWidget
{
    Q_OBJECT

public:
    PresetController(QWidget *parent = 0);
    ~PresetController();

signals:
    void createPreset();
    void setPreset(Preset *p);

private slots:
    void addPreset(Preset *p);
    void cellClicked(QModelIndex mi);

private:
    int m_instanceNum;
    QStandardItemModel *m_pm;
    QTableView *m_lv;

    QList<Preset*> m_pList;

    QPushButton *m_createPreset;

};

#endif // PRESETCONTROLLER_H
