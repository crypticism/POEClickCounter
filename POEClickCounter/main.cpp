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
    if (!json::has(settings, L"x_pos", json::JsonValueType::Number)) {
        File::update_settings(L"x_pos", json::value(500));
    }
    if (!json::has(settings, L"y_pos", json::JsonValueType::Number)) {
        File::update_settings(L"y_pos", json::value(500));
    }
    if (!json::has(settings, L"display_index", json::JsonValueType::Number)) {
        File::update_settings(L"display_index", json::value(1));
    }
    if (!json::has(settings, L"never_show", json::JsonValueType::Boolean)) {
        File::update_settings(L"never_show", json::value(false));
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
    if (!settings.GetNamedBoolean(L"never_show")) {
        StackedDisplayContainer::instance().show();
    }

    SystemTrayIcon icon;

    return a.exec();
}

