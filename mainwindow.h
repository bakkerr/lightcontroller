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
    void dockAll();
    void about();

private:
    QLabel *mainWidget;

    QVBoxLayout l0;
    QHBoxLayout l1;

    SingleController *master;
    QDockWidget *masterDockWidget;

    audioController *audio;

    QVector<LightController*> controllers;

    QToolBar *toolBar;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;


    QAction *viewAudioAction;
    QAction *viewMasterAction;
    QAction *dockAllAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;



};

#endif // MAINWINDOW_H
