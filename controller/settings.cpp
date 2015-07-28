#include "settings.h"

bool GLOBAL_settingsChanged = false;

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(tr(APPLICATION_NAME) + tr(" Settings"));
    QVBoxLayout *l0 = new QVBoxLayout();

    m_settingsManager = new QGroupBox(tr("Settings:"), this);

    QHBoxLayout *l3 = new QHBoxLayout();
    m_settingsDisplay = new QTreeWidget(this);
    m_settingsDisplay->setColumnCount(2);
    QStringList sl; sl.append(tr("Item")); sl.append(tr("Value"));
    m_settingsDisplay->setHeaderItem(new QTreeWidgetItem(sl));

    l3->addWidget(m_settingsDisplay);

    m_settingsManager->setLayout(l3);

    l0->addWidget(m_settingsManager);

    setMinimumWidth(250);

    displaySettings();

    setLayout(l0);

}

settingsDialog::~settingsDialog()
{

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

void settingsDialog::displaySettings()
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

settingsWidget::settingsWidget(QWidget *parent) :
    QDockWidget(parent)
{
    setWindowTitle(tr("Settings"));
    setMaximumWidth(150);
    setMaximumHeight(150);

    m_preferencesBox = new QGroupBox(tr("Preferences:"), this);

    QGridLayout *l1 = new QGridLayout();

    QLabel *udpRepeatLabel = new QLabel(tr("UDP resends:"), this);
    m_udpRepeat = new QSpinBox(this);
    m_udpRepeat->setMinimum(1);
    m_udpRepeat->setMaximum(3);
    m_udpRepeat->setValue(UDP_RESENDS_DEFAULT);
    connect(m_udpRepeat, SIGNAL(valueChanged(int)), this, SIGNAL(udpResends(int)));

    QLabel *wirelessRepeatLabel = new QLabel(tr("Wireless resends:"), this);
    m_wirelessRepeat = new QSpinBox(this);
    m_wirelessRepeat->setMinimum(1);
    m_wirelessRepeat->setMaximum(20);
    m_wirelessRepeat->setValue(WIRELESS_RESENDS_DEFAULT);
    connect(m_wirelessRepeat, SIGNAL(valueChanged(int)), this, SIGNAL(wirelessResends(int)));

    l1->addWidget(udpRepeatLabel, 1, 1, 1, 1);
    l1->addWidget(m_udpRepeat, 1, 2, 1, 1);
    l1->addWidget(wirelessRepeatLabel, 2, 1, 1, 1);
    l1->addWidget(m_wirelessRepeat, 2, 2, 1, 1);

    m_preferencesBox->setLayout(l1);

    setWidget(m_preferencesBox);
}
