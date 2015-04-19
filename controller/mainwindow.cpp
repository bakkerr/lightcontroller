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
    setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    /*
     * Create Mastercontroller (zone -1)
     * Controls all zones on all bridges.
     */
    master = new SingleController("Master", -1);
    masterDockWidget = new QDockWidget(tr("Master Controller"), this);
    masterDockWidget->setWidget(master);
    masterDockWidget->setMaximumWidth(200);
    masterDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::TopDockWidgetArea, masterDockWidget);

    /* Audio Controller */
    audio = new audioController(this);
    addDockWidget(Qt::TopDockWidgetArea, audio);

    /* Connect Audio to Master .*/
    connect(audio, SIGNAL(setRandomSame()), master, SLOT(setRandom()));
    connect(audio, SIGNAL(fade10()), master, SLOT(fade10()));
    connect(audio, SIGNAL(fade20()), master, SLOT(fade20()));

    setupActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();

    /* Center the window. */
    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    /* Create Bridge Discovery Dialog. */
    MiLightDiscover *d = new MiLightDiscover(this);
    connect(d, SIGNAL(selectedDevices(QStringList)), this, SLOT(setupControllers(QStringList)));
    d->exec();

}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    viewMasterAction = new QAction(tr("&Master Controller"), this);
    viewMasterAction->setCheckable(true);
    viewMasterAction->setChecked(true);
    connect(viewMasterAction, SIGNAL(toggled(bool)), masterDockWidget, SLOT(setVisible(bool)));

    viewStatusBarAction = new QAction(tr("&Status Bar"), this);
    viewStatusBarAction->setCheckable(true);
    viewStatusBarAction->setChecked(true);
    connect(viewStatusBarAction, SIGNAL(toggled(bool)), statusBar(), SLOT(setVisible(bool)));

    viewToolBarAction = new QAction(tr("&Tool Bar"), this);
    viewToolBarAction->setCheckable(true);
    viewToolBarAction->setChecked(true);

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
    viewMenu->addAction(viewMasterAction);

    viewControllersMenu = viewMenu->addMenu(tr("&Controllers"));
    viewMenu->addMenu(audio->viewAudioMenu);

    viewMenu->addSeparator();

    viewMenu->addAction(viewStatusBarAction);
    viewMenu->addAction(viewToolBarAction);

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
    toolBar->addAction(viewMasterAction);
    toolBar->addAction(audio->viewAudioAction);
    toolBar->addSeparator();
    toolBar->addAction(dockAllAction);
    toolBar->addSeparator();
    toolBar->addAction(aboutAction);
    toolBar->addSeparator();

    connect(viewToolBarAction, SIGNAL(toggled(bool)), toolBar, SLOT(setVisible(bool)));
}

void MainWindow::setupStatusBar()
{
    statusBar()->show();
    statusBar()->showMessage(tr("LightController - Roy Bakker - 2015 - http://github.com/bakkerr/lightcontroller"), 0);
}

void MainWindow::setupControllers(QStringList devices){


    for(int i = 0; i < devices.length(); i++){

        bool dummy = false;

        QString s = devices.at(i);
        QStringList split = s.split(',');

        if(split.length() < 2){
            qDebug() << "Invalid device String: " << s << endl;
            continue;
        }

        if(split.at(1).startsWith("DUMMY")) dummy = 1;

        qDebug() << "Found device IP: " << split.at(0) << "ID: " << split.at(1) << "Dummy: " << dummy << endl;

        LightController *lc = new LightController(tr("Controller %0").arg(QString::number(controllers.size() + 1)), split.at(0), this, dummy);

        for(int j = 1; j <= 4; j++){
            connect(audio, SIGNAL(setRandomAll()), lc->zones[j], SLOT(setRandom()));
            //connect(&special, SIGNAL(allRandom()), lc->zones[j], SLOT(setRandom()));
            //connect(&special, SIGNAL(allFade()), lc->zones[j], SLOT(enableFade()));
        }

        connect(master, SIGNAL(doColor(QColor, unsigned char)), lc->zones[0], SLOT(changeColor(QColor)));
        connect(master, SIGNAL(doBright(unsigned char, unsigned char)), lc->zones[0], SLOT(changeBright(unsigned char)));
        connect(master, SIGNAL(doOn(unsigned char)), lc->zones[0], SLOT(changeOn()));
        connect(master, SIGNAL(doOff(unsigned char)), lc->zones[0], SLOT(changeOff()));
        connect(master, SIGNAL(doWhite(unsigned char)), lc->zones[0], SLOT(changeWhite()));
        connect(master, SIGNAL(fadeEnabled()), lc->zones[0], SLOT(disableFade()));

        controllers.append(lc);

        addDockWidget(Qt::BottomDockWidgetArea, lc);
        if(controllers.size() > 1) tabifyDockWidget(controllers.first(), controllers.last());

        viewControllersMenu->addMenu(lc->viewControllerMenu);

    }

    /* Re-center window. FIXME: doesn't work. */
    //QWidget *w = window();
    //w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

}

void MainWindow::dockAll()
{
    masterDockWidget->setFloating(false);
    audio->setFloating(false);

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
