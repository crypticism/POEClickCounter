#pragma once

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>

#include "ui_SystemTrayIcon.h"

const QString ICON_PATH = QString::fromStdString("\\assets\\light_icon.ico");

const QString COUNT_LMB_AS_SKILL_USE_STRING = QString::fromStdString("Count LMB as Skill");
const QString DO_NOT_COUNT_LMB_AS_SKILL_USE_STRING = QString::fromStdString("Do Not Count LMB as Skill");

const QString LOCK_GUI_STRING = QString::fromStdString("Lock GUI");
const QString UNLOCK_GUI_STRING = QString::fromStdString("Unlock GUI");

const QString NEVER_SHOW_GUI_STRING = QString::fromStdString("Never Show GUI");
const QString SHOW_GUI_STRING = QString::fromStdString("Show GUI");

const QString REFRESH_KEYBINDS_STRING = QString::fromStdString("Refresh Keybinds");
const QString RESET_SESSION_STRING = QString::fromStdString("Reset Session Data");
const QString TOGGLE_GUI_MODE_STRING = QString::fromStdString("Toggle GUI Mode");

const QString QUIT_STRING = QString::fromStdString("Quit");

class SystemTrayIcon : public QDialog
{
	Q_OBJECT

public:
	SystemTrayIcon();
	~SystemTrayIcon();

	void setVisible(bool) override;

public slots:
	void refreshKeybinds();
	void toggleNeverShow();
	void toggleLock();
	void toggleCountLeftCLickAsSkill();

private:
	void createActions();
	void createTrayIcon();

	bool locked = true;
	bool never_show = false;

	QAction* toggleCountLeftClickAsSkillAction;
	QAction* resetSessionDataAction;
	QAction* toggleGUIModeAction;
	QAction* toggleGUILockAction;
	QAction* toggleNeverShowAction;
	QAction* refreshKeybindAction;

    QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};

#endif // SYSTEMTRAYICON_H
