#include "pch.h"

#include <QtWidgets/QApplication>

#include "gui/ClickDisplay.h"
#include "gui/SystemTrayIcon.h"
#include "io/file.h"
#include "io/ini.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    File::load_data();
    File::load_settings();
    INI::load_ini();

    // Initialize positions in settings if first launch
    json::JsonObject& settings = File::get_settings();
    if (!json::has(settings, L"x_pos", json::JsonValueType::Number)) {
        File::update_settings(L"x_pos", json::value(500));
    }
    if (!json::has(settings, L"y_pos", json::JsonValueType::Number)) {
        File::update_settings(L"y_pos", json::value(500));
    }

    QWidget::connect(&ClickDisplay::instance(), &ClickDisplay::handleEvent, &ClickDisplay::dispatchUpdate);
    ClickDisplay::instance().setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint });
    ClickDisplay::instance().show();

    SystemTrayIcon icon;

    return a.exec();
}
