#include "pch.h"

#include <Qt>
#include <QtWidgets/QApplication>

#include "gui/SystemTrayIcon.h"

#include "StackedDisplayContainer.h"

#include "io/file.h"
#include "io/ini.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    File::load_data();
    File::reset_session_data();
    File::load_settings();
    INI::load_ini();

    // Initialize positions in settings if first launch
    json::JsonObject& settings = File::get_settings();
    if (!json::has(settings, GUI_X_COORDINATE, json::JsonValueType::Number)) {
        File::update_settings(GUI_X_COORDINATE, json::value(500));
    }
    if (!json::has(settings, GUI_Y_COORDINATE, json::JsonValueType::Number)) {
        File::update_settings(GUI_Y_COORDINATE, json::value(500));
    }
    if (!json::has(settings, DISPLAY_INDEX, json::JsonValueType::Number)) {
        File::update_settings(DISPLAY_INDEX, json::value(1));
    }
    if (!json::has(settings, NEVER_SHOW_GUI, json::JsonValueType::Boolean)) {
        File::update_settings(NEVER_SHOW_GUI, json::value(false));
    }
    if (!json::has(settings, COUNT_LEFT_CLICK_AS_SKILL_USE, json::JsonValueType::Boolean)) {
        File::update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(false));
    }

    // Connect signals for the stacked display container
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::dispatchEvent, &StackedDisplayContainer::handleUpdate);
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::checkIsApplicationActive, &StackedDisplayContainer::isApplicationActive);
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::setApplicationActive, &StackedDisplayContainer::setIsCheckingActive);

    // Window settings to make the window bordless, always on top and have no taskbar thing
    StackedDisplayContainer::instance().setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow });
    // Window setting to allow the background to be transparent
    StackedDisplayContainer::instance().setAttribute(Qt::WA_TranslucentBackground);

    // Show the window by default if user has not set it to never show
    if (!settings.GetNamedBoolean(NEVER_SHOW_GUI)) {
        StackedDisplayContainer::instance().show();
    }

    SystemTrayIcon icon;

    return a.exec();
}

