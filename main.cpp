#include "restycagewindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RestyCageWindow w;
    w.show();
    return a.exec();
}
