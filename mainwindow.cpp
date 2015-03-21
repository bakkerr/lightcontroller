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
    setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 5px;}");


    QWidget *mw = new QWidget();
    statusBar()->show();
    statusBar()->showMessage(tr("Ready!"));
    QMenu *file = menuBar()->addMenu(tr("&File"));
    QAction *quitAction = new QAction(tr("E&xit"), this);
    file->addAction(quitAction);
    menuBar()->show();


    /* Center the window. */
    //QWidget *w = window();
    //w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    l1 = new QHBoxLayout();

    mc = new SingleController(-1);

    audio = new audioController(this);
    special = new specialButtons(this);
    l1->addWidget(mc);
    l1->addWidget(special);
    l1->addWidget(audio, 2);

    mw->setLayout(l1);

    setCentralWidget(mw);

    /* FIXME: Should change this to a dialog! */

    MiLightDiscover *d = new MiLightDiscover(this);
    connect(d, SIGNAL(selectedDevices(QStringList)), this, SLOT(setupControllers(QStringList)));

    /* Hack is only one controller is found, init it directly. */
    //if(d->done){
    //  setupControllers(1);
    //}
    /* Otherwise, wait for confirmation. */
    //else{
      //
    //}

}

MainWindow::~MainWindow()
{

}

void MainWindow::setupControllers(QStringList n){
    //delete d;

    LightController *lc;
    QDockWidget *dw;

    for(int i = 0; i < n.length(); i++){
        lc = new LightController();

        for(int j = 1; j <= 4; j++){
            connect(audio, SIGNAL(setRandomAll()), lc->zones[j], SLOT(setRandom()));
            connect(special, SIGNAL(allRandom()), lc->zones[j], SLOT(setRandom()));
            connect(special, SIGNAL(allFade()), lc->zones[j], SLOT(enableFade()));
        }

        connect(audio, SIGNAL(setRandomSame()), lc->zones[0], SLOT(setRandom()));
        connect(audio, SIGNAL(fade10()), lc->zones[0], SLOT(fade10()));
        connect(audio, SIGNAL(fade20()), lc->zones[0], SLOT(fade20()));

        dw = new QDockWidget(tr("Controller %0").arg(QString::number(i+1)));
        dw->setWidget(lc);

        controllers.append(lc);
        dockwidgets.append(dw);

        addDockWidget(Qt::BottomDockWidgetArea, dw);
        if(dockwidgets.size() > 1) tabifyDockWidget(dockwidgets.first(), dockwidgets.last());

    }

    /* Re-center window. FIXME: doesn't work. */
    //QWidget *w = window();
    //w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

}
