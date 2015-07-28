#include "presetcontroller.h"

PresetController::PresetController(QWidget *parent) :
    QDockWidget(tr("Preset Controller"), parent)
{
  m_instanceNum = 0;

  QWidget *mw = new QWidget(this);

  QVBoxLayout *l1 = new QVBoxLayout();

  m_createPresetButton = new QPushButton(tr("Create"), this);

  m_presetItemModel = new QStandardItemModel(0, 3, this);

  QStringList header;
  header.append(tr("Name"));
  header.append(tr("SET"));
  header.append(tr("DEL"));
  m_presetItemModel->setHorizontalHeaderLabels(header);

  m_presetTableView = new QTableView(this);
  m_presetTableView->setModel(m_presetItemModel);
  m_presetTableView->setColumnWidth(0, 120);
  m_presetTableView->setColumnWidth(1, 36);
  m_presetTableView->setColumnWidth(2, 36);

  l1->addWidget(m_createPresetButton);
  l1->addWidget(m_presetTableView);

  connect(m_createPresetButton, SIGNAL(clicked()), this, SIGNAL(createPreset()));
  connect(m_presetTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(cellClicked(QModelIndex)));
  connect(m_presetItemModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)));

  mw->setLayout(l1);

  setWidget(mw);

  setMinimumWidth(200);
  setMaximumWidth(200);

}

void PresetController::loadSettings(QSettings *s)
{
    s->beginGroup(tr("PresetController"));
    setVisible(s->value(tr("Visible"), tr("true")).toBool());

    int size = s->beginReadArray(tr("Presets"));

    for(int i = 0; i < size; i++){
        s->setArrayIndex(i);
        Preset *p = new Preset();
        p->loadSettings(s);
        addPreset(p);
    }

    s->endArray();
    s->endGroup();
}

void PresetController::saveSettings(QSettings *s)
{
    s->beginGroup(tr("PresetController"));
    s->setValue(tr("Visible"), isVisible());
    s->beginWriteArray(tr("Presets"));

    for(int i = 0; i < m_presetList.size(); i++){
        s->setArrayIndex(i);
        m_presetList.at(i)->saveSettings(s);
    }

    s->endArray();
    s->endGroup();
}

void PresetController::addPreset(Preset *p){

    p->setParent(this);

    m_instanceNum++;

    m_presetList.append(p);

    QList<QStandardItem*> newRow;

    if(p->m_name == QString()){
        p->m_name = tr("Preset ") + QString::number(m_instanceNum);
    }

    QStandardItem *name = new QStandardItem(p->m_name);
    QStandardItem *set  = new QStandardItem(tr("set"));
    QStandardItem *del  = new QStandardItem(tr("del"));
    name->setToolTip(tr("Preset saved on ") + p->m_date.toString(tr("dd-MM-yyyy hh:mm:ss")));
    set->setEditable(false);
    set->setTextAlignment(Qt::AlignCenter);
    del->setEditable(false);
    del->setTextAlignment(Qt::AlignCenter);

    newRow.append(name);
    newRow.append(set);
    newRow.append(del);

    m_presetItemModel->appendRow(newRow);

    GLOBAL_settingsChanged = true;
}

void PresetController::dataChanged(const QModelIndex &tl, const QModelIndex &br)
{
    qDebug() << "Called..." << endl;

    if(tl.row() != br.row() || tl.column() != tl.column()){
        qDebug() << "Editing of multiple cells is not supported..." << endl;
        return;
    }

    if(tl.column() != 0){
        qDebug() << "Other column than column 0 changed??? Confused..." << endl;
        return;
    }

    int row = tl.row();

    m_presetList.at(row)->setName(m_presetItemModel->data(tl).toString());

}

void PresetController::cellClicked(QModelIndex mi)
{
    int col = mi.column();
    int row = mi.row();

    switch(col){
      case 0:
        break;
      case 1:
        emit(setPreset(m_presetList.at(row)));
        break;
      case 2:
      {
        QMessageBox::StandardButton mb = QMessageBox::question(this,
                                                               tr("Delete Preset?"),
                                                               tr("Are you sure you want to delete this preset?"),
                                                               QMessageBox::Yes | QMessageBox::No,
                                                               QMessageBox::No
                                                               );

        if(mb == QMessageBox::Yes){
            m_presetList.removeAt(row);
            m_presetItemModel->removeRow(row);
            GLOBAL_settingsChanged = true;
        }
        break;
      }
      case 3:
        break;
    }
}
