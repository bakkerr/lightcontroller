#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr("Light Controller"));
    setStyleSheet("QGroupBox{border:1px solid gray;border-radius:5px;margin-top: 1ex;} QGroupBox::title{subcontrol-origin: margin;subcontrol-position:top center;padding:0 5px;}");

    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

    MiLightDiscover *d = new MiLightDiscover();

    connect(d, SIGNAL(userSelected()), this, SLOT(setupControllers()));

    setCentralWidget(d);


    qDebug() << "Test5" << endl;
}

void MainWindow::setupControllers(){
    lc = new LightController();

    setCentralWidget(lc);

    QWidget *w = window();
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w->size(), qApp->desktop()->availableGeometry()));

}
