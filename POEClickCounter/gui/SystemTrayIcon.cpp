#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"

#include "ClickDisplay.h"
#include "../io/file.h"
#include "../io/ini.h"

SystemTrayIcon::SystemTrayIcon()
{
    // Set ui state based on saved setting
    json::JsonObject& settings = File::get_settings();
    never_show = settings.GetNamedBoolean(L"never_show");

	createActions();
	createTrayIcon();

    if (never_show) {
        toggleNeverShowAction->setText(tr("&Show GUI"));
    }

    QIcon icon = QIcon("gui\\light_icon.ico");
    trayIcon->setIcon(icon);
    trayIcon->show();
}

SystemTrayIcon::~SystemTrayIcon()
{
}


void SystemTrayIcon::setVisible(bool visible) {
    quitAction->setEnabled(visible);
    QDialog::setVisible(visible);
}

void SystemTrayIcon::refreshKeybinds() {
    INI::load_ini();
}

void SystemTrayIcon::createActions()
{
    toggleNeverShowAction = new QAction(tr("&Never Show GUI"), this);
    connect(toggleNeverShowAction, &QAction::triggered, &ClickDisplay::instance(), &ClickDisplay::neverShow);
    connect(toggleNeverShowAction, &QAction::triggered, this, &SystemTrayIcon::toggleNeverShow);

    refreshAction = new QAction(tr("&Refresh Keybinds"), this);
    connect(refreshAction, &QAction::triggered, this, &SystemTrayIcon::refreshKeybinds);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SystemTrayIcon::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(toggleNeverShowAction);
    trayIconMenu->addAction(refreshAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void SystemTrayIcon::toggleNeverShow()
{
    never_show = !never_show;

    if (never_show) {
        toggleNeverShowAction->setText(tr("&Show GUI"));
    }
    else {
        toggleNeverShowAction->setText(tr("&Never Show GUI"));
    }

    File::update_settings(L"never_show", json::value(never_show));
}