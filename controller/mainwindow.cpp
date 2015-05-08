#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    /* Window Title */
    setWindowTitle(tr("Light Controller"));

    /*
     * "Fix" Groupbox layout
     *
     *  FIXME: Make this an option.
     */
#ifdef LC_FIX_LAYOUT
    setStyleSheet("QDockWidget::title {background: darkgray;} QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 5px;}");
#endif

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
    addDockWidget(Qt::RightDockWidgetArea, presetController);
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

    delete d;

}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    viewStatusBarAction = new QAction(tr("&Status Bar"), this);
    viewStatusBarAction->setCheckable(true);
    viewStatusBarAction->setChecked(true);
    connect(viewStatusBarAction, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));

    dockAllAction = new QAction(tr("&Dock All"), this);
    connect(dockAllAction, SIGNAL(triggered()), this, SLOT(dockAll()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::setupMenuBar()
{
    /* File Menu */
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    /* View Menu */
    viewMenu = menuBar()->addMenu(tr("&View"));

    viewMenu->addAction(masterDockWidget->toggleViewAction());

    viewMenu->addSeparator();

    viewMenu->addAction(audio->toggleViewAction());
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
    statusBar()->showMessage(tr("LightController - Roy Bakker - 2015 - http://github.com/bakkerr/lightcontroller"), 0);
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
        usleep(100000);
        master->setBrightExt(18);
    }


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
    QMessageBox::about(this, tr("About Lightcontroller"),
                       tr("A Lightcontroller for MiLight/LimitlessLed/etc WiFi Bridges"
                          "<br />"
                          "<br />"
                          "<b>Author:</b><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Roy Bakker (2015)<br>"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:roy@roybakker.nl\">roy@roybakker.nl</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http:/www.roybakker.nl>\">www.roybakker.nl</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"https:/github.com/bakkerr/lightcontroller>\">github.com/bakkerr/lightcontroller</a><br />"
                          "<br />"
                          "<br />"
                          "<b>Credits:</b><br />"
                          "&nbsp;&nbsp; This project makes use of:<br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- a modified version of <a href=\"https://github.com/mguentner/libbeat\">Libbeat</a> by Maximilian Güntner<br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- a modified version of <a href=\"https://github.com/liuyanghejerry/Qt-Plus/tree/master/develop/ColorWheel\">ColorWheel</a> by liuyanghejerry<br />"
                          "<br />"
                          "<br />"
                          "<b>Note:</b> This project is in Alpha stage and will most probably contain several bugs and/or stability issues...<br />"
                        ));
}
