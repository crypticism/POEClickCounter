#include "pch.h"

#include <Qt>
#include <QtWidgets/QApplication>

#include "gui/SystemTrayIcon.h"

#include "StackedDisplayContainer.h"

#include "io/data.h"
#include "io/ini.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Data::load_data();
    Data::reset_session_data();
    Data::load_settings();
    INI::load_ini();

    // Connect signals for the stacked display container
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::dispatchEvent, &StackedDisplayContainer::handleUpdate);
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::checkIsApplicationActive, &StackedDisplayContainer::isApplicationActive);
    QWidget::connect(&StackedDisplayContainer::instance(), &StackedDisplayContainer::setApplicationActive, &StackedDisplayContainer::setIsCheckingActive);

    // Window settings to make the window bordless, always on top and have no taskbar thing
    StackedDisplayContainer::instance().setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow });
    // Window setting to allow the background to be transparent
    StackedDisplayContainer::instance().setAttribute(Qt::WA_TranslucentBackground);

    // Show the window by default if user has not set it to never show
    json::JsonObject settings = Data::get_settings();
    if (!settings.GetNamedBoolean(NEVER_SHOW_GUI)) {
        StackedDisplayContainer::instance().show();
    }

    SystemTrayIcon icon;

    return a.exec();
}

