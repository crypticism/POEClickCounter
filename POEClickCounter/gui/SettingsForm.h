#pragma once

#include <QWidget>
#include "ui_SettingsForm.h"


const QString LOCK_GUI_STRING = QString::fromStdString("Lock GUI");
const QString UNLOCK_GUI_STRING = QString::fromStdString("Unlock GUI");

class SettingsForm : public QWidget
{
	Q_OBJECT

public:
	static SettingsForm& instance();
	SettingsForm(QWidget *parent = Q_NULLPTR);
	~SettingsForm();

public slots:
	void showSettings();

private slots:
	void neverShowStateChanged(int);
	void countLeftClickStateChanged();
	void displayIndexStateChanged();
	void toggleLocked();
	void saveSettings();

private:
	Ui::SettingsForm *ui;

	bool movement_locked = true;

	void closeEvent(QCloseEvent*) override;
};
