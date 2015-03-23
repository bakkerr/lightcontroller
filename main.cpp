#include "mainwindow.h"

#include "discover.h"

#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    //discover();

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
