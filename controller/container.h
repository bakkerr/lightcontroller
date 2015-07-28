#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>
#include <QWidget>
#include <QDockWidget>

#include <QHBoxLayout>

#include "default_values.h"
#include "singlecontroller.h"

class container : public QDockWidget
{
    Q_OBJECT

public:
    explicit container(QString name, quint16 id, QWidget *parent = 0);
    ~container();

    quint16 id()   { return m_id;   }
    QString name() { return m_name; }

public slots:
    void addController(SingleController *lc);
    void removeController(SingleController *lc);

private:
    quint16 m_id;
    QString m_name;

    QHBoxLayout *layout;
    QWidget *mainWidget;
    QList<SingleController*> controllers;
};

#endif // CONTAINER_H
