#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"
#include "SettingsForm.h"
#include "StackedDisplayContainer.h"
#include "../io/data.h"
#include "../io/ini.h"

SystemTrayIcon::SystemTrayIcon()
{
	createActions();
	createTrayIcon();

    TCHAR buffer[1024];
    GetModuleFileName(NULL, buffer, 1024);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

    QString path = QString::fromStdWString(std::wstring(buffer).substr(0, pos)) + ICON_PATH;
    QIcon icon = QIcon(path);
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
