#include "pch.h"

#include <Qt>
#include <QtWidgets/QApplication>

#include "gui/APM.h"
#include "gui/SettingsForm.h"
#include "gui/StackedDisplayContainer.h"
#include "gui/SystemTrayIcon.h"

#include "io/data.h"
#include "io/ini.h"

#include "utils/utils.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    Data::load_data();
    Data::reset_session_data();
    Data::load_settings();
    INI::load_ini();

    // Show the window by default if user has not set it to never show
    json::JsonObject settings = Data::get_settings();
    if (!settings.GetNamedBoolean(NEVER_SHOW_GUI)) {
        StackedDisplayContainer::instance().show();
    }

    APM apm;
    SystemTrayIcon icon;
    SettingsForm form;

    return application.exec();
}

