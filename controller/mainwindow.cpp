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
    setStyleSheet("QDockWidget::title {background: darkgray;} QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 5px;}");

    setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    /*
     * Create Mastercontroller (zone -1)
     * Controls all zones on all bridges.
     */
    master = new SingleController(-1);
    masterDockWidget = new QDockWidget(tr("Master Controller"), this);
    masterDockWidget->setWidget(master);
    masterDockWidget->setMaximumWidth(200);
    masterDockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::TopDockWidgetArea, masterDockWidget);

    /* Audio Controller */
    audio = new audioController(this);
    addDockWidget(Qt::TopDockWidgetArea, audio);

    /* Fixme: Move from here */
    statusBar()->show();
    statusBar()->showMessage(tr("Ready!"));

    fileMenu = menuBar()->addMenu(tr("&File"));
    exitAction = new QAction(tr("E&xit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));

    viewAudioAction = new QAction(tr("&Audio Controller"), this);
    viewAudioAction->setCheckable(true);
    viewAudioAction->setChecked(true);
    connect(viewAudioAction, SIGNAL(toggled(bool)), audio, SLOT(setVisible(bool)));
    viewMenu->addAction(viewAudioAction);

    viewMasterAction = new QAction(tr("&Master Controller"), this);
    viewMasterAction->setCheckable(true);
    viewMasterAction->setChecked(true);
    connect(viewMasterAction, SIGNAL(toggled(bool)), masterDockWidget, SLOT(setVisible(bool)));
    viewMenu->addAction(viewMasterAction);

    dockAllAction = new QAction(tr("&Dock All"), this);
    connect(dockAllAction, SIGNAL(triggered()), this, SLOT(dockAll()));
    viewMenu->addAction(dockAllAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    helpMenu->addAction(aboutAction);
    aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAction);

    menuBar()->show();

    toolBar = addToolBar(tr("Tools"));
    toolBar->addSeparator();
    toolBar->addAction(viewMasterAction);
    toolBar->addAction(viewAudioAction);
    toolBar->addSeparator();
    toolBar->addAction(dockAllAction);
    toolBar->addSeparator();
    toolBar->addAction(aboutAction);
    toolBar->addSeparator();

    /* Center the window. */
    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    mainWidget = new QLabel(tr("Light Controller"));
    mainWidget->setAlignment(Qt::AlignCenter);
    //setCentralWidget(mainWidget);

    /* Create Bridge Discovery Dialog. */
    MiLightDiscover *d = new MiLightDiscover(this);
    connect(d, SIGNAL(selectedDevices(QStringList)), this, SLOT(setupControllers(QStringList)));
    d->exec();

    /*if (QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "Systray available!" << endl;
        QMenu *trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(exitAction);
        QSystemTrayIcon *tray = new QSystemTrayIcon(this);
        tray->setIcon(QIcon("/tmp/heart.svg"));
        tray->setContextMenu(trayIconMenu);

        tray->show();
    }*/

}

MainWindow::~MainWindow()
{

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

        connect(master, SIGNAL(colorChange(QColor, unsigned char)), lc->zones[0], SLOT(changeColor(QColor)));
        connect(master, SIGNAL(brightChange(unsigned char, unsigned char)), lc->zones[0], SLOT(changeBright(unsigned char)));
        connect(master, SIGNAL(doOn(unsigned char)), lc->zones[0], SLOT(changeOn()));
        connect(master, SIGNAL(doOff(unsigned char)), lc->zones[0], SLOT(changeOff()));
        connect(master, SIGNAL(doWhite(unsigned char)), lc->zones[0], SLOT(changeWhite()));
        connect(master, SIGNAL(fadeEnabled()), lc->zones[0], SLOT(disableFade()));

        connect(audio, SIGNAL(setRandomSame()), lc->zones[0], SLOT(setRandom()));
        connect(audio, SIGNAL(fade10()), lc->zones[0], SLOT(fade10()));
        connect(audio, SIGNAL(fade20()), lc->zones[0], SLOT(fade20()));

        controllers.append(lc);

        addDockWidget(Qt::BottomDockWidgetArea, lc);
        if(controllers.size() > 1) tabifyDockWidget(controllers.first(), controllers.last());

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
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Roy Bakker (2015)<br>"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"mailto:roy@roybakker.nl\">roy@roybakker.nl</a><br />"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"http:/www.roybakker.nl>\">www.roybakker.nl</a><br/>"));
}
