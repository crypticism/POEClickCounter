#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>

#include "SystemTrayIcon.h"

#include "StackedDisplayContainer.h"
#include "../io/data.h"
#include "../io/ini.h"

SystemTrayIcon::SystemTrayIcon()
{
    // Set ui state based on saved setting
    json::JsonObject& settings = Data::get_settings();
    never_show = settings.GetNamedBoolean(NEVER_SHOW_GUI);

	createActions();
	createTrayIcon();

    if (never_show) {
        toggleNeverShowAction->setText(SHOW_GUI_STRING);
    }

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
    resetSessionDataAction = new QAction(RESET_SESSION_STRING, this);
    connect(resetSessionDataAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::resetSessionData);

    json::JsonObject settings = Data::get_settings();
    bool count_left_click_as_skill = settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE);

    toggleCountLeftClickAsSkillAction = new QAction(COUNT_LMB_AS_SKILL_USE_STRING, this);
    if (count_left_click_as_skill) {
        toggleCountLeftClickAsSkillAction->setText(DO_NOT_COUNT_LMB_AS_SKILL_USE_STRING);
    }
    connect(toggleCountLeftClickAsSkillAction, &QAction::triggered, this, &SystemTrayIcon::toggleCountLeftCLickAsSkill);

    toggleGUIModeAction = new QAction(TOGGLE_GUI_MODE_STRING, this);
    connect(toggleGUIModeAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::toggleGUIMode);

    toggleGUILockAction = new QAction(UNLOCK_GUI_STRING, this);
    connect(toggleGUILockAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::toggleLock);
    connect(toggleGUILockAction, &QAction::triggered, this, &SystemTrayIcon::toggleLock);

    bool never_show = settings.GetNamedBoolean(NEVER_SHOW_GUI);
    toggleNeverShowAction = new QAction(NEVER_SHOW_GUI_STRING, this);
    if (never_show) {
        toggleNeverShowAction->setText(SHOW_GUI_STRING);
    }
    connect(toggleNeverShowAction, &QAction::triggered, &StackedDisplayContainer::instance(), &StackedDisplayContainer::neverShow);
    connect(toggleNeverShowAction, &QAction::triggered, this, &SystemTrayIcon::toggleNeverShow);

    refreshKeybindAction = new QAction(REFRESH_KEYBINDS_STRING, this);
    connect(refreshKeybindAction, &QAction::triggered, this, &SystemTrayIcon::refreshKeybinds);

    quitAction = new QAction(QUIT_STRING, this);
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
        toggleNeverShowAction->setText(SHOW_GUI_STRING);
    }
    else {
        toggleNeverShowAction->setText(NEVER_SHOW_GUI_STRING);
    }

    Data::update_settings(NEVER_SHOW_GUI, json::value(never_show));
}

void SystemTrayIcon::toggleLock()
{
    locked = !locked;

    if (locked) {
        toggleGUILockAction->setText(UNLOCK_GUI_STRING);
    }
    else {
        toggleGUILockAction->setText(LOCK_GUI_STRING);
    }
    Data::save_settings();
}

void SystemTrayIcon::toggleCountLeftCLickAsSkill() {
    json::JsonObject settings = Data::get_settings();
    bool count_left_click_as_skill = !settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE);
    Data::update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(count_left_click_as_skill));

    if (count_left_click_as_skill) {
        toggleCountLeftClickAsSkillAction->setText(DO_NOT_COUNT_LMB_AS_SKILL_USE_STRING);
    }
    else {
        toggleCountLeftClickAsSkillAction->setText(COUNT_LMB_AS_SKILL_USE_STRING);
    }
}