#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"

#include "StackedDisplayContainer.h"
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

    TCHAR buffer[1024];
    GetModuleFileName(NULL, buffer, 1024);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

    QString path = QString::fromStdWString(std::wstring(buffer).substr(0, pos)) + QString::fromStdString("\\assets\\light_icon.ico");
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
    resetSessionDataAction = new QAction(tr("&Reset Session Data"), this);
    connect(resetSessionDataAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::resetSessionData);

    json::JsonObject settings = File::get_settings();
    bool count_left_click_as_skill = settings.GetNamedBoolean(L"count_left_click_as_skill");

    toggleCountLeftClickAsSkillAction = new QAction(tr("&Count LMB as Skill"), this);
    if (count_left_click_as_skill) {
        toggleCountLeftClickAsSkillAction->setText(tr("&Do Not Count LMB as Skill"));
    }
    connect(toggleCountLeftClickAsSkillAction, &QAction::triggered, this, &SystemTrayIcon::toggleCountLeftCLickAsSkill);

    toggleGUIModeAction = new QAction(tr("&Toggle GUI Mode"), this);
    connect(toggleGUIModeAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::toggleGUIMode);

    toggleGUILockAction = new QAction(tr("&Unlock GUI"), this);
    connect(toggleGUILockAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::toggleLock);
    connect(toggleGUILockAction, &QAction::triggered, this, &SystemTrayIcon::toggleLock);

    bool never_show = settings.GetNamedBoolean(L"never_show");
    toggleNeverShowAction = new QAction(tr("&Never Show GUI"), this);
    if (never_show) {
        toggleNeverShowAction->setText(tr("&Show GUI"));
    }
    connect(toggleNeverShowAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::neverShow);
    connect(toggleNeverShowAction, &QAction::triggered, this, &SystemTrayIcon::toggleNeverShow);

    refreshKeybindAction = new QAction(tr("&Refresh Keybinds"), this);
    connect(refreshKeybindAction, &QAction::triggered, this, &SystemTrayIcon::refreshKeybinds);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SystemTrayIcon::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(resetSessionDataAction);
    trayIconMenu->addAction(refreshKeybindAction);

    trayIconMenu->addSeparator();

    trayIconMenu->addAction(toggleCountLeftClickAsSkillAction);
    trayIconMenu->addAction(toggleGUIModeAction);
    trayIconMenu->addAction(toggleGUILockAction);
    trayIconMenu->addAction(toggleNeverShowAction);

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

void SystemTrayIcon::toggleLock()
{
    locked = !locked;

    if (locked) {
        toggleGUILockAction->setText(tr("&Unlock GUI"));
    }
    else {
        toggleGUILockAction->setText(tr("&Lock GUI"));
    }
    File::save_settings();
}

void SystemTrayIcon::toggleCountLeftCLickAsSkill() {
    json::JsonObject settings = File::get_settings();
    bool count_left_click_as_skill = !settings.GetNamedBoolean(L"count_left_click_as_skill");
    File::update_settings(L"count_left_click_as_skill", json::value(count_left_click_as_skill));

    if (count_left_click_as_skill) {
        toggleCountLeftClickAsSkillAction->setText(tr("&Do Not Count LMB as Skills"));
    }
    else {
        toggleCountLeftClickAsSkillAction->setText(tr("&Count LMB as Skills"));
    }
}