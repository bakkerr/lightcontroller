#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lightcontroller.h"
#include "discover.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:

private slots:
    void setupControllers(QStringList n);

private:
    QVBoxLayout *l0;
    QHBoxLayout *l1;

    MiLightDiscover *d;

    SingleController *mc;
    audioController *audio;
    specialButtons *special;

    QVector<LightController*> controllers;
    QVector<QDockWidget*> dockwidgets;


};

#endif // MAINWINDOW_H
