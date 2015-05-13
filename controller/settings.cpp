#include "settings.h"

bool GLOBAL_settingsChanged = false;

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr(APPLICATION_NAME) + tr(" Settings"));
    QVBoxLayout *l0 = new QVBoxLayout();

    m_preferencesBox = new QGroupBox(tr("Preferences:"), this);

    QVBoxLayout *l1 = new QVBoxLayout();

    QHBoxLayout *l2 = new QHBoxLayout();
    m_udpSleepLabel = new QLabel(tr("UDP Sleep"));
    m_udpSleepLabel->setToolTip(tr("Microsecond sleep after each UDP command"));
    m_udpSleepText = new QLineEdit(tr("100"), this);
    l2->addWidget(m_udpSleepLabel);
    l2->addWidget(m_udpSleepText);

    m_saveButton = new QPushButton(tr("Save"), this);

    l1->addLayout(l2);
    l1->addWidget(m_saveButton);

    m_preferencesBox->setLayout(l1);

    m_settingsManager = new QGroupBox(tr("Settings:"), this);

    QHBoxLayout *l3 = new QHBoxLayout();
    m_settingsDisplay = new QTreeWidget(this);
    m_settingsDisplay->setColumnCount(2);
    QStringList sl; sl.append(tr("Item")); sl.append(tr("Value"));
    m_settingsDisplay->setHeaderItem(new QTreeWidgetItem(sl));

    l3->addWidget(m_settingsDisplay);

    m_settingsManager->setLayout(l3);

    l0->addWidget(m_preferencesBox);
    l0->addWidget(m_settingsManager);

    setMinimumWidth(250);

    displaySettings();

    setLayout(l0);

}

QTreeWidgetItem *settingsDialog::__newNesting(QSettings *s, QTreeWidgetItem *twi)
{
    QStringList keys = s->childKeys();

    for(int i = 0; i < keys.size(); i++){
        QString k = keys.at(i);
        QStringList sl;
        sl.append(k);
        if(s->value(k).canConvert(QVariant::String)){
            sl.append(s->value(k).toString());
        }
        else{
            sl.append(tr("[Binary]"));
        }
        twi->addChild(new QTreeWidgetItem(sl));
    }

    keys = s->childGroups();

    for(int i = 0; i < keys.size(); i++){
        QString k = keys.at(i);
        QStringList sl;
        sl.append(k);
        sl.append(tr(""));
        QTreeWidgetItem *ntwi = new QTreeWidgetItem(sl);
        s->beginGroup(k);
        twi->addChild(__newNesting(s, ntwi));
        s->endGroup();
    }

    return twi;
}

void settingsDialog::displaySettings(const QString setting)
{

    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME));

    QStringList keys = s->childKeys();

    for(int i = 0; i < keys.size(); i++){
        QString k = keys.at(i);
        QStringList sl;
        sl.append(k);
        if(s->value(k).canConvert(QVariant::String)){
            sl.append(s->value(k).toString());
        }
        else{
            sl.append(tr("[Binary]"));
        }
        m_settingsDisplay->addTopLevelItem(new QTreeWidgetItem(sl));
    }

    keys = s->childGroups();

    for(int i = 0; i < keys.size(); i++){
        QString k = keys.at(i);
        QStringList sl;
        sl.append(k);
        sl.append(tr(""));
        QTreeWidgetItem *twi = new QTreeWidgetItem(sl);
        s->beginGroup(k);
        m_settingsDisplay->addTopLevelItem(__newNesting(s, twi));
        s->endGroup();
    }



}

settingsDialog::~settingsDialog()
{

}

