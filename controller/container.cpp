#include "container.h"

container::container(QString name, quint16 id, QWidget *parent):
    QDockWidget(name, parent)
{
    m_name = name;
    m_id = id;
    mainWidget = new QWidget(this);
    layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    mainWidget->setLayout(layout);

    setWidget(mainWidget);
}

container::~container()
{

}

void container::addController(SingleController *lc)
{
    controllers.append(lc);
    layout->addWidget(lc);
}

void container::removeController(SingleController *lc){
    controllers.removeOne(lc);
    layout->removeWidget(lc);
}

