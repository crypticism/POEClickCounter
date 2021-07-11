#include "pch.h"

#include <Qt>
#include <QtWidgets/QApplication>

#include "gui/SystemTrayIcon.h"

#include "io/data.h"
#include "io/ini.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    Data::load_data();
    Data::reset_session_data();
    Data::load_settings();
    INI::load_ini();

    SystemTrayIcon icon;

    return application.exec();
}
