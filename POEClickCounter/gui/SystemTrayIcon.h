#pragma once

#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QSystemTrayIcon>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>

#include "ClickDisplay.h"
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

private:
	void createActions();
	void createTrayIcon();

	bool never_show = false;

	QAction* toggleNeverShowAction;
	QAction* refreshAction;

    QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};
