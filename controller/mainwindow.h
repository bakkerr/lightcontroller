#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>

#include "default_values.h"
#include "settings.h"
#include "addcontrollerdialog.h"
#include "audiocontroller.h"
#include "presetcontroller.h"
#include "container.h"
#include "milightupdsender.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    SingleController* getControllerByID(quint16 id);
    container* getContainerByID(quint16 id);

signals:
    void presetAvailable(Preset *p);

public slots:
    void closeEvent(QCloseEvent *event);

private slots:
    void addController(quint16 id, QString name, quint16 remote, QList<quint16> slave_ids, quint16 containerID);
    void addContainer(QString name, quint16 id);

    void loadSettings(QString settingsName = tr(DEFAULT_SAVE_NAME));
    void saveSettings(QString settingsName = tr(DEFAULT_SAVE_NAME));
    void loadSettingsFrom();
    void saveSettingsAs();
    void clearSettings();

    void connectAudioController(int id);

    void getPreset();
    void setPreset(Preset *p);
    void showSettingsDialog();
    void showAddControllerDialog();
    void showAddContainerDialog();
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

    MiLightUPDsender *udp;

    audioController *audio;

    PresetController *presetController;

    settingsWidget *settings;

    QList<SingleController*> controllers;
    QList<container*> containers;

    QToolBar *toolBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *saveSettingsAction;
    QAction *saveSettingsAsAction;
    QAction *loadSettingsAction;
    QAction *clearSettingsAction;

    QAction *viewStatusBarAction;
    QAction *dockAllAction;

    QAction *settingsAction;
    QAction *addControllerAction;
    QAction *addContainerAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;

};

#endif // MAINWINDOW_H
