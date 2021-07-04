#pragma once

#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>

#include "ui_SystemTrayIcon.h"

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
