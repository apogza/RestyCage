#include "forms/resty_cage_window.h"

#include <QApplication>
#include <QUrl>
#include <QIcon>

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
    a.setApplicationName("RestyCage");
    a.setOrganizationName("AnonymousSoft");
    a.setWindowIcon(QIcon(":/resources/256-apps-restycage.png"));

    RestyCageWindow w;

    w.show();

    return a.exec();
}
