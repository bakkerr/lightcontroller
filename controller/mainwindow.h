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
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *event);

private slots:
    void setupControllers(const QStringList &devices, bool setDefaults);
    void loadSettings(QString settingsName = tr(DEFAULT_SAVE_NAME));
    void saveSettings(QString settingsName = tr(DEFAULT_SAVE_NAME));
    void saveSettingsAs();
    void clearSettings();
    void getPreset();
    void setPreset(Preset *p);
    void showSettingsDialog();
    void settingsChanged() { qDebug() << "Called" << endl; GLOBAL_settingsChanged = true; }
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
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *saveSettingsAction;
    QAction *saveSettingsAsAction;
    QAction *clearSettingsAction;

    QAction *viewStatusBarAction;
    QAction *dockAllAction;

    QAction *settingsAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;

};

#endif // MAINWINDOW_H
