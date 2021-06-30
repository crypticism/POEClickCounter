#include "gui/POEClickCounter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    POEClickCounter w;
    w.show();
    return a.exec();
}
