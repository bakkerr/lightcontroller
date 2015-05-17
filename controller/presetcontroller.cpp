#include "presetcontroller.h"

PresetController::PresetController(QWidget *parent) :
    QDockWidget(tr("Preset Controller"), parent)
{
  m_instanceNum = 0;

  QWidget *mw = new QWidget(this);

  QVBoxLayout *l1 = new QVBoxLayout();

  m_createPreset = new QPushButton(tr("Create"), this);

  m_tw = new QTableWidget(0, 3, this);

  QStringList header;
  header.append(tr("Name"));
  header.append(tr("SET"));
  header.append(tr("DEL"));
  m_tw->setHorizontalHeaderLabels(header);

  m_tw->setColumnWidth(0, 120);
  m_tw->setColumnWidth(1, 36);
  m_tw->setColumnWidth(2, 36);

  l1->addWidget(m_createPreset);
  l1->addWidget(m_tw);

  connect(m_createPreset, SIGNAL(clicked()), this, SIGNAL(createPreset()));
  connect(m_tw, SIGNAL(cellClicked(int, int)), this, SLOT(cellClicked(int, int)));
  connect(m_tw, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));

  mw->setLayout(l1);

  setWidget(mw);

  setMinimumWidth(200);
  setMaximumWidth(200);

}

void PresetController::loadSettings(QSettings *s)
{
    s->beginGroup(tr("Presets"));
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
    s->beginGroup(tr("Presets"));
    s->setValue(tr("Visible"), isVisible());
    s->beginWriteArray(tr("Presets"));

    for(int i = 0; i < m_pList.size(); i++){
        s->setArrayIndex(i);
        m_pList.at(i)->saveSettings(s);
    }

    s->endArray();
    s->endGroup();
}

void PresetController::addPreset(Preset *p){

    p->setParent(this);

    m_instanceNum++;

    m_pList.append(p);

    if(p->m_name == QString()){
        p->m_name = tr("Preset ") + QString::number(m_instanceNum);
    }

    QTableWidgetItem *name = new QTableWidgetItem(p->m_name);
    QTableWidgetItem *set  = new QTableWidgetItem(tr("set"));
    QTableWidgetItem *del  = new QTableWidgetItem(tr("del"));
    name->setToolTip(tr("Preset saved on ") + p->m_date.toString(tr("dd-MM-yyyy hh:mm:ss")));
    set->setFlags(set->flags() & ~Qt::ItemIsEditable);
    set->setTextAlignment(Qt::AlignCenter);
    del->setFlags(del->flags() & ~Qt::ItemIsEditable);
    del->setTextAlignment(Qt::AlignCenter);

    int row = m_tw->rowCount();
    m_tw->insertRow(row);
    m_tw->setItem(row, 0, name);
    m_tw->setItem(row, 1, set);
    m_tw->setItem(row, 2, del);

    GLOBAL_settingsChanged = true;
}

void PresetController::cellChanged(int row, int column)
{
    if(column == 0){
        m_pList.at(row)->setName(m_tw->item(row, column)->text());
    }
}

void PresetController::cellClicked(int row, int column)
{

    switch(column){
      case 0:
        break;
      case 1:
        emit(setPreset(m_pList.at(row)));
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
            m_pList.removeAt(row);
            m_tw->removeRow(row);
            GLOBAL_settingsChanged = true;
        }
        break;
      }
      default:
        break;
    }
}

PresetController::~PresetController()
{

}

