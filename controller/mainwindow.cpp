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
    connect(audio, SIGNAL(controllerChanged(int)), this, SLOT(connectAudioController(int)));
    addDockWidget(Qt::TopDockWidgetArea, audio);

    presetController = new PresetController(this);
    presetController->setMinimumWidth(230);
    connect(presetController, SIGNAL(createPreset()), this, SLOT(getPreset()));
    connect(this, SIGNAL(presetAvailable(Preset*)), presetController, SLOT(addPreset(Preset*)));
    connect(presetController, SIGNAL(setPreset(Preset*)), this, SLOT(setPreset(Preset*)));
    addDockWidget(Qt::TopDockWidgetArea, presetController);

    settings = new settingsWidget(this);
    connect(settings, SIGNAL(udpResends(int)), udp, SLOT(setUdpResends(int)));
    connect(settings, SIGNAL(wirelessResends(int)), udp, SLOT(setWirelessResends(int)));
    addDockWidget(Qt::TopDockWidgetArea, settings);

    /* Center the window. */
    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    setupActions();
    setupToolBar();
    setupMenuBar();
    setupStatusBar();

    loadSettings();

    if(containers.empty()){
        addContainer(tr("Main"), 1);
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    saveSettingsAction = new QAction(tr("&Save Settings"), this);
    connect(saveSettingsAction, SIGNAL(triggered()), this, SLOT(saveSettings()));

    saveSettingsAsAction = new QAction(tr("Save Settings &as..."), this);
    connect(saveSettingsAsAction, SIGNAL(triggered()), this, SLOT(saveSettingsAs()));

    loadSettingsAction = new QAction(tr("&Load Settings"), this);
    connect(loadSettingsAction, SIGNAL(triggered()), this, SLOT(loadSettingsFrom()));

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

    addContainerAction = new QAction(tr("Add &Container"), this);
    connect(addContainerAction, SIGNAL(triggered()), this, SLOT(showAddContainerDialog()));

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
    fileMenu->addAction(saveSettingsAsAction);
    fileMenu->addAction(loadSettingsAction);
    fileMenu->addAction(clearSettingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    /* Edit Menu */
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(settingsAction);
    editMenu->addAction(addControllerAction);
    editMenu->addAction(addContainerAction);

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
    toolBar->addAction(dockAllAction);
    toolBar->addSeparator();
    toolBar->addAction(aboutAction);
    toolBar->addSeparator();

    toolBar->addSeparator();
    toolBar->addAction(audio->toggleViewAction());
    toolBar->addAction(presetController->toggleViewAction());
    toolBar->addAction(settings->toggleViewAction());
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

void MainWindow::loadSettings(QString settingsName)
{
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);

    s->beginGroup(settingsName);

    int size = s->beginReadArray(tr("Containers"));

    for(int i = 0; i < size; i++){
        s->setArrayIndex(i);
        quint16 id = s->value(tr("id"), 0x00).toUInt();
        QString name = s->value(tr("name"), tr("name")).toString();

        addContainer(name, id);
    }

    s->endArray();

    size = s->beginReadArray(tr("Controllers"));

    for(int i = 0; i < size; i++){
        s->setArrayIndex(i);

        quint16 id = s->value(tr("id"), 0x00).toUInt();
        QString name = s->value(tr("name"), tr("name")).toString();
        quint16 remote = s->value(tr("remote"), 0x00).toUInt();
        quint16 containerID = s->value(tr("containerID"), 0x01).toUInt();

        int slave_size = s->beginReadArray(tr("Slaves"));
        QList<quint16> slaves;
        for(int j = 0; j < slave_size; j++){
            s->setArrayIndex(j);
            slaves.append(s->value(tr("id"), 0x00).toUInt());
        }
        s->endArray();

        addController(id, name, remote, slaves, containerID);
    }

    s->endArray();

    presetController->loadSettings(s);

    audio->loadSettings(s);

    s->endGroup();

    GLOBAL_settingsChanged = false;

    s->sync();
}


void MainWindow::saveSettings(QString settingsName)
{
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
    s->beginGroup(settingsName);

    s->beginGroup(tr("Info"));
    s->setValue(tr("version"), tr(APPLICATION_VERSION));
#if defined(__DATE__) && defined(__TIME__)
    s->setValue(tr("build_date"), tr(__DATE__) + tr(" ") + tr(__TIME__));
#endif
    s->setValue(tr("saved"), QDateTime::currentDateTime());
    s->endGroup();

    s->beginWriteArray(tr("Containers"));

    for(int i = 0; i < containers.count(); i++){
        s->setArrayIndex(i);
        container *c = containers.at(i);
        s->setValue(tr("id"), c->id());
        s->setValue(tr("name"), c->name());
    }

    s->endArray();

    s->beginWriteArray(tr("Controllers"));

    for(int i = 0; i < controllers.count(); i++){
        s->setArrayIndex(i);

        SingleController *lc = controllers.at(i);
        s->setValue(tr("id"), lc->id());
        s->setValue(tr("name"), lc->name());
        s->setValue(tr("remote"), lc->remote());
        s->setValue(tr("containerID"), lc->container());

        s->beginWriteArray(tr("Slaves"));
        QList<SingleController*> slaves = lc->slaves();
        for(int j = 0; j < slaves.count(); j++){
            s->setArrayIndex(j);
            s->setValue(tr("id"), slaves.at(j)->id());
        }
        s->endArray();
    }

    s->endArray();

    presetController->saveSettings(s);

    audio->saveSettings(s);

    s->endGroup();

    s->sync();

    GLOBAL_settingsChanged = false;

}

void MainWindow::loadSettingsFrom(){
    bool ok = false;

    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
    QStringList items = s->childGroups();

    if(items.isEmpty()){
        QMessageBox::warning(this, tr("Settings"), tr("There are no settings to load!"));
        return;
    };

    QString settings = QInputDialog::getItem(this, tr("Select settings to load"), tr("Settings:"), items, items.indexOf(DEFAULT_SAVE_NAME), false, &ok);

    if(ok){
        //loadSettings(settings);
    }
}

void MainWindow::saveSettingsAs()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Set name"), tr("Name:"), QLineEdit::Normal, tr(""), &ok);
    if(ok){
        if(name.isEmpty()){
            saveSettingsAs();
            return;
        }
        QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
        if(s->childGroups().contains(name)){
            QMessageBox::StandardButton mb = QMessageBox::question(this,
                tr("Overwrite settings?"),
                tr("Settings with the name \"") + name + tr("\" already exist. Do you want to overwrite them?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
            if(mb == QMessageBox::No){
                return;
            }
        }

        saveSettings(name);
    }
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

void MainWindow::connectAudioController(int id){

    qDebug() << id << endl;

    foreach(SingleController *lc, controllers){
        audio->disconnect(lc);
    }

    if(id > 0){
        SingleController *lc = getControllerByID((quint16)id);

        connect(audio, SIGNAL(fade10()), lc, SLOT(fade10()));
        connect(audio, SIGNAL(fade20()), lc, SLOT(fade20()));
        connect(audio, SIGNAL(setRandomSame()), lc, SLOT(setRandomExtSame()));
        connect(audio, SIGNAL(setRandomDifferent()), lc, SLOT(setRandomExtDifferent()));
        connect(audio, SIGNAL(flash()), lc, SLOT(flash()));
        connect(audio, SIGNAL(flashRandom()), lc, SLOT(flashRandom()));
    }

}

void MainWindow::addController(quint16 id, QString name, quint16 remote, QList<quint16> slave_ids, quint16 containerID)
{
    QList<SingleController *> slaves;
    foreach(SingleController *l, controllers){
        if(l->id() == id){
            QMessageBox::critical(this, tr("Error!"), tr("Trying to add controller with the same id!"));
            return;
        }

        if(slave_ids.contains(l->remote())){
            slaves.append(l);
        }
    }

    SingleController *lc = new SingleController(id, name, remote, slaves, containerID);

    if(remote > 0){
        connect(lc, SIGNAL(doOn(quint16)), udp, SLOT(setOn(quint16)));
        connect(lc, SIGNAL(doOff(quint16)), udp, SLOT(setOff(quint16)));
        connect(lc, SIGNAL(doWhite(quint16)), udp, SLOT(setWhite(quint16)));
        connect(lc, SIGNAL(doColor(QColor, quint16)), udp, SLOT(setColor(QColor, quint16)));
        connect(lc, SIGNAL(doBright(quint8, quint16)), udp, SLOT(setBright(quint8, quint16)));

        connect(lc, SIGNAL(doPair(quint16)), udp, SLOT(pair(quint16)));
        connect(lc, SIGNAL(doUnPair(quint16)), udp, SLOT(unPair(quint16)));
    }

    foreach(SingleController *slave, slaves){
        connect(lc, SIGNAL(fadeEnabled()), slave, SLOT(enableFade()));
        connect(lc, SIGNAL(fadeDisabled()), slave, SLOT(disableFade()));
    }

    controllers.append(lc);
    getContainerByID(containerID)->addController(lc);

    audio->updateControllers(controllers);

}

void MainWindow::addContainer(QString name, quint16 id)
{
    int newid = 0;
    foreach(container *c, containers){
        if(c->id() > newid){
            newid = c->id();
        }
        if(c->id() == id){
            QMessageBox::warning(this, tr(APPLICATION_NAME), tr("Cannot add container with existing ID!"));
            return;
        }
    }
    newid++;

    if(id == 0){
        id = newid;
    }

    container *c = new container(name, id, this);
    containers.append(c);

    addDockWidget(Qt::BottomDockWidgetArea, c);

    toolBar->addAction(c->toggleViewAction());
}

void MainWindow::showSettingsDialog()
{
    settingsDialog *s = new settingsDialog(this);
    s->exec();
}

void MainWindow::showAddControllerDialog()
{
    addControllerDialog *d = new addControllerDialog(controllers, containers, this);
    connect(d, SIGNAL(addController(quint16, QString, quint16, QList<quint16>, quint16)), this, SLOT(addController(quint16, QString, quint16, QList<quint16>, quint16)));
    d->exec();
}

void MainWindow::showAddContainerDialog()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Container"), tr("Name:"), QLineEdit::Normal, tr(""), &ok);
    if(ok){
        addContainer(name, 0);
    }
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

SingleController* MainWindow::getControllerByID(quint16 id)
{
    foreach(SingleController *lc, controllers){
        if(lc->id() == id){
            return lc;
        }
    }

    return NULL;
}

container* MainWindow::getContainerByID(quint16 id)
{
    foreach(container *c, containers){
        if(c->id() == id){
            return c;
        }
    }

    return NULL;
}

void MainWindow::getPreset()
{
    Preset *p = new Preset(this);

    foreach(SingleController *lc, controllers){
        PresetLC *plc = lc->getPreset();
        p->addController(plc);
    }

    emit presetAvailable(p);
}

void MainWindow::setPreset(Preset *p)
{
    foreach(PresetLC *plc, p->lcs){
        getControllerByID(plc->id())->setPreset(plc, true);
    }
}

void MainWindow::dockAll()
{
    audio->setFloating(false);
    presetController->setFloating(false);
    settings->setFloating(false);

    foreach(container *c, containers){
        c->setFloating(false);
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
