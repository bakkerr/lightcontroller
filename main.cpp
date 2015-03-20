#include "lightcontroller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LightController w;
    w.show();

    return a.exec();
}
