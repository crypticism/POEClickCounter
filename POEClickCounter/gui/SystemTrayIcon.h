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

protected:/*
	void closeEvent(QCloseEvent*) override;*/

private slots:
	//void setIcon(int index);
	//void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void createActions();
	void createTrayIcon();

    QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};
