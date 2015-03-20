#include "lightcontroller.h"

#include "discover.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    discover();
    return 0;

    QApplication a(argc, argv);

    LightController w;
    w.show();

    return a.exec();
}
