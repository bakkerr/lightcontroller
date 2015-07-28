#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    /* Window Title */
    setWindowTitle(tr(APPLICATION_NAME));

    /* Set sticky places for dockwidgets */
    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    udp = new MiLightUPDsender(tr("192.168.1.12"), 8899, this);

    /* Audio Controller */
    audio = new audioController(this);
    addDockWidget(Qt::TopDockWidgetArea, audio);

    presetController = new PresetController(this);
    presetController->setMinimumWidth(230);
    addDockWidget(Qt::TopDockWidgetArea, presetController);
    connect(presetController, SIGNAL(createPreset()), this, SLOT(getPreset()));
    connect(this, SIGNAL(presetAvailable(Preset*)), presetController, SLOT(addPreset(Preset*)));
    connect(presetController, SIGNAL(setPreset(Preset*)), this, SLOT(setPreset(Preset*)));

    /* Center the window. */
    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    setupActions();
    setupToolBar();
    setupMenuBar();
    setupStatusBar();

    QList<quint16> slaves;
    for(int i = 1; i <= 5; i++){
        addController(tr("L") + QString::number(i), i, QList<quint16>());
        slaves.append(i);
    }
    addController(tr("Master"), 0x0044, slaves);

    //loadSettings();

}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    saveSettingsAction = new QAction(tr("&Save Settings"), this);
    connect(saveSettingsAction, SIGNAL(triggered()), this, SLOT(saveSettings()));

    clearSettingsAction = new QAction(tr("&Clear Settings"), this);
    connect(clearSettingsAction, SIGNAL(triggered()), this, SLOT(clearSettings()));

    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    viewStatusBarAction = new QAction(tr("&Status Bar"), this);
    viewStatusBarAction->setCheckable(true);
    viewStatusBarAction->setChecked(true);
    connect(viewStatusBarAction, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));

    settingsAction = new QAction(tr("&Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    addControllerAction = new QAction(tr("&Add Controller"), this);
    connect(addControllerAction, SIGNAL(triggered()), this, SLOT(showAddControllerDialog()));

    dockAllAction = new QAction(tr("&Dock All"), this);
    connect(dockAllAction, SIGNAL(triggered()), this, SLOT(dockAll()));

    aboutAction = new QAction(tr("&About ") + tr(APPLICATION_NAME), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::setupMenuBar()
{
    /* File Menu */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(saveSettingsAction);
    fileMenu->addAction(clearSettingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    /* Edit Menu */
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(settingsAction);
    editMenu->addAction(addControllerAction);

    /* View Menu */
    viewMenu = menuBar()->addMenu(tr("&View"));

    viewMenu->addMenu(audio->viewAudioMenu);

    viewMenu->addAction(presetController->toggleViewAction());

    viewMenu->addSeparator();

    foreach(SingleController *lc, controllers){
        viewMenu->addAction(lc->viewControllerAction);
    }

    viewMenu->addSeparator();

    viewMenu->addAction(viewStatusBarAction);
    viewMenu->addAction(toolBar->toggleViewAction());

    viewMenu->addSeparator();

    viewMenu->addAction(dockAllAction);

    /* Help Menu */
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    menuBar()->show();
}

void MainWindow::setupToolBar()
{
    toolBar = addToolBar(tr("Tools"));
    toolBar->addSeparator();
    toolBar->addAction(audio->toggleViewAction());
    toolBar->addAction(presetController->toggleViewAction());
    toolBar->addSeparator();


    foreach(SingleController *lc, controllers){
        toolBar->addAction(lc->toggleViewAction());
    }

    toolBar->addSeparator();
    toolBar->addAction(dockAllAction);
    toolBar->addSeparator();
    toolBar->addAction(aboutAction);
    toolBar->addSeparator();
}

void MainWindow::setupStatusBar()
{
    statusBar()->show();
    statusBar()->showMessage(tr(APPLICATION_NAME)    + tr(" v")   +
                             tr(APPLICATION_VERSION) + tr(" - ") +
                             tr(APPLICATION_AUTHOR)  + tr(" - ") +
                             tr(APPLICATION_YEAR)    + tr(" - ") +
                             tr(APPLICATION_URL)
                             , 0);
}

void MainWindow::loadSettings()
{
/*
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);

    s->beginGroup(tr("MainWindow"));

    master->setName(s->value(tr("MasterControllerName"), tr("Master")).toString());

    int size = s->beginReadArray(tr("LightControllers"));

    for(int i = 0; i < controllers.size(); i++){
        LightController *lc = controllers.at(i);
        for(int j = 0; j < size; j++){
            s->setArrayIndex(j);
            if(s->value(tr("id"), tr("")).toString() == lc->id()){
                lc->loadSettings(s);
            }
        }
    }

    s->endArray();

    presetController->loadSettings(s);

    audio->loadSettings(s);

    s->endGroup();

    GLOBAL_settingsChanged = false;

    s->sync();
*/
}

void MainWindow::saveSettings()
{
/*
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
    s->beginGroup(tr("MainWindow"));
    s->setValue(tr("MasterControllerName"), master->name());
    s->setValue(tr("MasterControllerVisible"), master->isVisible());


    s->beginWriteArray(tr("LightControllers"));

    for(int i = 0; i < controllers.size(); i++){
        s->setArrayIndex(i);
        controllers.at(i)->saveSettings(s);
    }

    s->endArray();

    presetController->saveSettings(s);

    audio->saveSettings(s);

    s->endGroup();

    s->sync();

    GLOBAL_settingsChanged = false;
*/
}

void MainWindow::clearSettings()
{
    QMessageBox::StandardButton mb = QMessageBox::question(this,
                                                       tr("Clear all settings?"),
                                                       tr("Do you really want to clear all settings?"),
                                                       QMessageBox::Yes | QMessageBox::No,
                                                       QMessageBox::No
                                                       );

    if(mb == QMessageBox::Yes){
        QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
        s->clear();
        s->sync();
        GLOBAL_settingsChanged = false;
    }

}

void MainWindow::addController(QString name, quint16 remote, QList<quint16> slave_ids)
{
    QList<SingleController *> slaves;
    foreach(SingleController *l, controllers){
        if(slave_ids.contains(l->remote())){
            slaves.append(l);
        }
    }

    SingleController *lc = new SingleController(name, remote, slaves);

    if(remote > 0){
        connect(lc, SIGNAL(doOn(quint16)), udp, SLOT(setOn(quint16)));
        connect(lc, SIGNAL(doOff(quint16)), udp, SLOT(setOff(quint16)));
        connect(lc, SIGNAL(doWhite(quint16)), udp, SLOT(setWhite(quint16)));
        connect(lc, SIGNAL(doColor(QColor, quint16)), udp, SLOT(setColor(QColor, quint16)));
        connect(lc, SIGNAL(doBright(quint8, quint16)), udp, SLOT(setBright(quint8, quint16)));

        connect(lc, SIGNAL(doPair(quint16)), udp, SLOT(pair(quint16)));
        connect(lc, SIGNAL(doUnPair(quint16)), udp, SLOT(unPair(quint16)));
    }

    controllers.append(lc);

    addDockWidget(Qt::BottomDockWidgetArea, lc);
}

void MainWindow::showSettingsDialog()
{
    settingsDialog *s = new settingsDialog(this);
    s->exec();
}

void MainWindow::showAddControllerDialog()
{
    addControllerDialog *d = new addControllerDialog(controllers, this);
    connect(d, SIGNAL(addController(QString,quint16,QList<quint16>)), this, SLOT(addController(QString,quint16,QList<quint16>)));
    d->exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(GLOBAL_settingsChanged){
        QMessageBox::StandardButton mb = QMessageBox::question(this,
                                                           tr("Save settings before closing?"),
                                                           tr("Some settings were modified. Do you want to save these settings?"),
                                                           QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::No
                                                           );

        if(mb == QMessageBox::Yes){
            saveSettings();
        }
    }

    event->accept();

}

void MainWindow::getPreset()
{
/*
    Preset *p = new Preset(this);

    p->master = master->getPreset();

    QVectorIterator<LightController*> i(controllers);
    while(i.hasNext()){
        LightController *lc = i.next();
        PresetLC * plc = lc->getPreset();
        p->addController(plc);
    }

    emit presetAvailable(p);
*/
}

void MainWindow::setPreset(Preset *p)
{

    /* Need to do something with the mastercontroller... */

    /* Set all controllers. */
/*
    QVectorIterator<PresetLC*> pi(p->lcs);
    while(pi.hasNext()){
        PresetLC * plc = pi.next();

        QVectorIterator<LightController*> ci(controllers);
        while (ci.hasNext()) {
            LightController *lc = ci.next();
            if(lc->id() == plc->m_id){
                lc->setPreset(plc);
            }
        }

    }
*/
}

void MainWindow::dockAll()
{
    audio->setFloating(false);
    presetController->setFloating(false);

    foreach(SingleController *lc, controllers){
        lc->setFloating(false);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ") + tr(APPLICATION_NAME),
                       tr("A Lightcontroller for MiLight/LimitlessLed/etc WiFi Bridges"
                          "<br />"
                          "<br />"
                          "<b>Author:</b><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;") + tr(APPLICATION_AUTHOR) + tr(" (") + tr(APPLICATION_YEAR) + tr(")<br>"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:") + tr(APPLICATION_AUTHOR_EMAIL) + tr("\">") + tr(APPLICATION_AUTHOR_EMAIL) + tr("</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://") + tr(APPLICATION_AUTHOR_URL) + tr(">\">") + tr(APPLICATION_AUTHOR_URL) + tr("</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http://") + tr(APPLICATION_URL) + tr(">\">") + tr(APPLICATION_URL) + tr("</a><br />"
                          "<br />"
                          "<br />"
                          "<b>Credits:</b><br />"
                          "&nbsp;&nbsp; This project makes use of:<br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- UDP commandset as described on <a href=\"http://www.limitlessled.com/dev\">www.limitlessled.com/dev</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- a modified version of <a href=\"https://github.com/mguentner/libbeat\">Libbeat</a> by Maximilian Güntner<br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- a modified version of <a href=\"https://github.com/liuyanghejerry/Qt-Plus/tree/master/develop/ColorWheel\">ColorWheel</a> by liuyanghejerry<br />"
                          "<br />"
                          "<br />"
                          "<b>Note:</b><br />"
                          "This project is in Alpha stage and will most probably contain several bugs and/or stability issues...<br />"
                          "<br /><br /><b>") + tr(APPLICATION_NAME) + tr(" v") + tr(APPLICATION_VERSION) + tr("</b>")
#if defined(__DATE__) && defined(__TIME__)
                          + tr(" (Build: ") + tr(__DATE__) + tr(" ") + tr(__TIME__) + tr(")")
#endif

                        );
}
