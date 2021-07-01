#include "pch.h"

#include <QtWidgets/QApplication>

#include "gui/ClickDisplay.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget::connect(&ClickDisplay::instance(), &ClickDisplay::handleEvent, &ClickDisplay::dispatchUpdate);
    ClickDisplay::instance().setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint });
    ClickDisplay::instance().show();
    return a.exec();
}
