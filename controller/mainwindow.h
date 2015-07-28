#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>

#include "default_values.h"
#include "settings.h"
#include "addcontrollerdialog.h"
#include "lightcontroller.h"
#include "audiocontroller.h"
#include "presetcontroller.h"
#include "discover.h"
#include "milightupdsender.h"

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
    void addController(QString name, quint16 remote, QList<quint16> slave_ids);
    void loadSettings();
    void saveSettings();
    void clearSettings();
    void getPreset();
    void setPreset(Preset *p);
    void showSettingsDialog();
    void showAddControllerDialog();
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

    QList<SingleController*> controllers;

    QToolBar *toolBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *saveSettingsAction;
    QAction *clearSettingsAction;

    QAction *viewStatusBarAction;
    QAction *dockAllAction;

    QAction *settingsAction;
    QAction *addControllerAction;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *exitAction;

};

#endif // MAINWINDOW_H
