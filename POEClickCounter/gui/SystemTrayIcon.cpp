#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"

#include "../io/ini.h"

SystemTrayIcon::SystemTrayIcon()
{
	createActions();
	createTrayIcon();

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
    refreshAction = new QAction(tr("&Refresh Keybinds"), this);
    connect(refreshAction, &QAction::triggered, this, &SystemTrayIcon::refreshKeybinds);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SystemTrayIcon::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(refreshAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);


    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}