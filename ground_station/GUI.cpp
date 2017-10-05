#include "gswidget.h"
#include <QApplication>

int runGUI(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GSWidget w;
    w.show();

    return a.exec();
}
