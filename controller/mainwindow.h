#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include "lightcontroller.h"
#include "audiocontroller.h"
#include "presetcontroller.h"
#include "discover.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void presetAvailable(Preset *p);

public slots:

private slots:
    void setupControllers(const QStringList &devices);
    void getPreset();
    void setPreset(Preset *p);
    void dockAll();
    void about();

private:
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();

    QVBoxLayout l0;
    QHBoxLayout l1;

    SingleController *master;
    QDockWidget *masterDockWidget;

    audioController *audio;

    PresetController *presetController;

    QVector<LightController*> controllers;

    QToolBar *toolBar;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *viewStatusBarAction;
    QAction *dockAllAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;

};

#endif // MAINWINDOW_H
