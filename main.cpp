#include "forms/resty_cage_window.h"

#include <QApplication>
#include <QUrl>
#include<QDirIterator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*
    QFile darkStyleFile (":/resty_cage/resources/darkstyle.qss");

    if (darkStyleFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&darkStyleFile);
        a.setStyleSheet(stream.readAll());
    }
    */


    RestyCageWindow w;
    w.show();

    return a.exec();
}
