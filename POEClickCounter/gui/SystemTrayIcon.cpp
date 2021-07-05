#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"
#include "SettingsForm.h"
#include "StackedDisplayContainer.h"
#include "../io/data.h"
#include "../io/ini.h"
#include "../utils/utils.h"

SystemTrayIcon::SystemTrayIcon()
{
	createActions();
	createTrayIcon();
    
    trayIcon->setIcon(get_icon());
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
    openSettingsAction = new QAction(SETTINGS_STRING, this);
    connect(openSettingsAction, &QAction::triggered, &SettingsForm::instance(), &SettingsForm::showSettings);

    resetSessionDataAction = new QAction(RESET_SESSION_STRING, this);
    connect(resetSessionDataAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::resetSessionData);

    refreshKeybindAction = new QAction(REFRESH_KEYBINDS_STRING, this);
    connect(refreshKeybindAction, &QAction::triggered, this, &SystemTrayIcon::refreshKeybinds);

    quitAction = new QAction(QUIT_STRING, this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SystemTrayIcon::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openSettingsAction);

    trayIconMenu->addSeparator();

    trayIconMenu->addAction(resetSessionDataAction);
    trayIconMenu->addAction(refreshKeybindAction);

    trayIconMenu->addSeparator();

    trayIconMenu->addAction(quitAction);


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
