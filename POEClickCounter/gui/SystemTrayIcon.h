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



const QString SETTINGS_STRING = QString::fromStdString("Settings");

const QString REFRESH_KEYBINDS_STRING = QString::fromStdString("Refresh Keybinds");
const QString RESET_SESSION_STRING = QString::fromStdString("Reset Session Data");

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

private:
	void createActions();
	void createTrayIcon();

	QAction* openSettingsAction;

	QAction* resetSessionDataAction;
	QAction* refreshKeybindAction;

    QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};

#endif // SYSTEMTRAYICON_H
