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

    /*
     * Create Mastercontroller (zone -1)
     * Controls all zones on all bridges.
     */
    master = new SingleController("Master", -1, this);
    masterDockWidget = new QDockWidget(tr("Master Controller"), this);
    masterDockWidget->setWidget(master);
    masterDockWidget->setMaximumWidth(200);
    addDockWidget(Qt::TopDockWidgetArea, masterDockWidget);

    /* Audio Controller */
    audio = new audioController(this);
    addDockWidget(Qt::TopDockWidgetArea, audio);

    /* Connect Audio to Master .*/
    connect(audio, SIGNAL(setRandomSame()), master, SLOT(setRandomExt()));
    connect(audio, SIGNAL(fade10()), master, SLOT(fade10Ext()));
    connect(audio, SIGNAL(fade20()), master, SLOT(fade20Ext()));

    presetController = new PresetController(this);
    presetController->setMinimumWidth(230);
    addDockWidget(Qt::TopDockWidgetArea, presetController);
    connect(presetController, SIGNAL(createPreset()), this, SLOT(getPreset()));
    connect(this, SIGNAL(presetAvailable(Preset*)), presetController, SLOT(addPreset(Preset*)));
    connect(presetController, SIGNAL(setPreset(Preset*)), this, SLOT(setPreset(Preset*)));

    /* Center the window. */
    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    /* Create Bridge Discovery Dialog. */
    MiLightDiscover *d = new MiLightDiscover(this);
    connect(d, SIGNAL(selectedDevices(QStringList, bool)), this, SLOT(setupControllers(QStringList, bool)));
    d->exec();

    setupActions();
    setupToolBar();
    setupMenuBar();
    setupStatusBar();

    loadSettings();

    delete d;

}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    saveSettingsAction = new QAction(tr("&Save Settings as \"") + tr(DEFAULT_SAVE_NAME) + tr("\""), this);
    connect(saveSettingsAction, SIGNAL(triggered()), this, SLOT(saveSettings()));

    saveSettingsAsAction = new QAction(tr("&Save Settings as..."), this);
    connect(saveSettingsAsAction, SIGNAL(triggered()), this, SLOT(saveSettingsAs()));

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
    fileMenu->addAction(clearSettingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    /* Edit Menu */
    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(settingsAction);

    /* View Menu */
    viewMenu = menuBar()->addMenu(tr("&View"));

    viewMenu->addAction(masterDockWidget->toggleViewAction());

    viewMenu->addMenu(audio->viewAudioMenu);

    viewMenu->addAction(presetController->toggleViewAction());

    viewMenu->addSeparator();

    QVectorIterator<LightController*> i(controllers);
    while(i.hasNext()){
        LightController * lc = i.next();
        viewMenu->addMenu(lc->viewControllerMenu);
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
    toolBar->addAction(masterDockWidget->toggleViewAction());
    toolBar->addAction(audio->toggleViewAction());
    toolBar->addAction(presetController->toggleViewAction());
    toolBar->addSeparator();

    QVectorIterator<LightController*> i(controllers);
    while(i.hasNext()){
        toolBar->addAction(i.next()->toggleViewAction());
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

void MainWindow::loadSettings(QString settingsName)
{
    QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);

    s->beginGroup(settingsName);

    s->beginGroup(tr("MasterController"));
    master->setName(s->value(tr("name"), tr("Master")).toString());
    master->setVisible(s->value(tr("visible"), tr("true")).toBool());
    s->endGroup();

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

    s->beginGroup(tr("MasterController"));
    s->setValue(tr("name"), master->name());
    s->setValue(tr("visible"), master->isVisible());
    s->endGroup();

    s->beginWriteArray(tr("LightControllers"));

    for(int i = 0; i < controllers.size(); i++){
        s->setArrayIndex(i);
        controllers.at(i)->saveSettings(s);
    }

    s->endArray();

    presetController->saveSettings(s);

    audio->saveSettings(s);

    //s->beginGroup(tr("Preset"));
    //Preset *p = getPreset();
    //p->saveSettings(s);
    //s->endGroup();

    /* End settings and sync */
    s->endGroup();

    s->sync();

    GLOBAL_settingsChanged = false;
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
        QMessageBox::No);

    if(mb == QMessageBox::Yes){
        QSettings *s = new QSettings(tr(APPLICATION_COMPANY), tr(APPLICATION_NAME), this);
        s->clear();
        s->sync();
        GLOBAL_settingsChanged = false;
    }

}

void MainWindow::setupControllers(const QStringList &devices, bool setDefaults){


    for(int i = 0; i < devices.length(); i++){

        bool dummy = false;

        QString s = devices.at(i);
        QStringList split = s.split(',');

        if(split.length() < 2){
            qDebug() << "Invalid device String: " << s << endl;
            continue;
        }

        if(split.at(1).startsWith("DUMMY")) dummy = true;

        qDebug() << "Found device IP: " << split.at(0) << "ID: " << split.at(1) << "Dummy: " << dummy << endl;

        LightController *lc = new LightController(split.at(0), split.at(1), controllers.size() + 1, dummy, this);

        for(int j = 1; j <= 4; j++){
            connect(audio, SIGNAL(setRandomAll()), lc->zones[j], SLOT(setRandomExt()));
        }

        connect(master, SIGNAL(doColor(QColor, unsigned char)), lc->zones[0], SLOT(setColorExt(QColor)));
        connect(master, SIGNAL(doBright(unsigned char, unsigned char)), lc->zones[0], SLOT(setBrightExt(unsigned char)));
        connect(master, SIGNAL(doOn(unsigned char)), lc->zones[0], SLOT(setOnExt()));
        connect(master, SIGNAL(doOff(unsigned char)), lc->zones[0], SLOT(setOffExt()));
        connect(master, SIGNAL(doWhite(unsigned char)), lc->zones[0], SLOT(setWhiteExt()));
        connect(master, SIGNAL(fadeEnabled()), lc->zones[0], SLOT(disableFade()));

        controllers.append(lc);

        addDockWidget(Qt::BottomDockWidgetArea, lc);
        if(controllers.size() > 1) tabifyDockWidget(controllers.first(), controllers.last());

    }

    if(setDefaults){
        master->setColorExt(Qt::blue);
        usleep(150000);
        master->setBrightExt(18);
    }
}

void MainWindow::showSettingsDialog()
{
    settingsDialog *s = new settingsDialog(this);
    s->exec();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key()){
      case Qt::Key_0:
        master->setOffExt();
        break;
      case Qt::Key_1:
        master->setOnExt();
        break;
      case Qt::Key_F:
      {
        QColor c = master->color();
        if(c.hue() != QColor(Qt::white).hue()){
            master->setWhiteExt();
            master->setColorExt(c);
        }
        else{
            master->setRandomExt();
            master->setWhiteExt();
        }
        break;
      }
      case Qt::Key_A:
        //audio->
        break;
      case Qt::Key_P:
        getPreset();
        break;
      case Qt::Key_R:
        master->setRandomExt();
        break;
      case Qt::Key_T:
        master->state() ? master->setOffExt() : master->setOnExt();
        break;
      case Qt::Key_W:
        master->setWhiteExt();
        break;
      case Qt::Key_Up:
        master->setBrightExt(master->brightness() + 1);
        break;
      case Qt::Key_Down:
        master->setBrightExt(master->brightness() - 1);
        break;
      case Qt::Key_Left:
        master->fadeExt(-1);
        break;
      case Qt::Key_Right:
        master->fadeExt(1);
        break;
      default:
        break;
    }

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(GLOBAL_settingsChanged){
        QMessageBox::StandardButton mb = QMessageBox::question(this,
                                                           tr("Save settings before closing?"),
                                                           tr("Some settings were modified. Do you want to save these settings to \"") + tr(DEFAULT_SAVE_NAME) + tr("\"?"),
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
    Preset *p = new Preset(this);

    p->master = master->getPreset();

    QVectorIterator<LightController*> i(controllers);
    while(i.hasNext()){
        LightController *lc = i.next();
        PresetLC * plc = lc->getPreset();
        p->addController(plc);
    }

    emit presetAvailable(p);
}

void MainWindow::setPreset(Preset *p)
{

    /* Need to do something with the mastercontroller... */

    /* Set all controllers. */
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

}

void MainWindow::dockAll()
{
    masterDockWidget->setFloating(false);
    audio->setFloating(false);
    presetController->setFloating(false);

    for(int i = 0; i < controllers.size(); i++){
        controllers.at(i)->setFloating(false);
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
