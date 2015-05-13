#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>

#include "default_values.h"
#include "settings.h"
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
    void closeEvent(QCloseEvent *event);

private slots:
    void setupControllers(const QStringList &devices, bool setDefaults);
    void getPreset();
    void setPreset(Preset *p);
    void showSettingsDialog();
    void settingsChanged() { GLOBAL_settingsChanged = true; }
    void dockAll();
    void about();

private:
    void setupActions();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void loadSettings();
    void saveSettings();

    QVBoxLayout l0;
    QHBoxLayout l1;

    SingleController *master;
    QDockWidget *masterDockWidget;

    audioController *audio;

    PresetController *presetController;

    QVector<LightController*> controllers;

    QToolBar *toolBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *viewStatusBarAction;
    QAction *dockAllAction;

    QAction *settingsAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;

};

#endif // MAINWINDOW_H
