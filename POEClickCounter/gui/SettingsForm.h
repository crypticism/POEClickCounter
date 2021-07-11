#pragma once

#include <QWidget>
#include "ui_SettingsForm.h"


const QString LOCK_GUI_STRING = QString::fromStdString("Lock GUI");
const QString UNLOCK_GUI_STRING = QString::fromStdString("Unlock GUI");

class SettingsForm : public QWidget
{
	Q_OBJECT

public:
	SettingsForm(QWidget *parent = Q_NULLPTR);
	~SettingsForm();

public slots:
	void showSettings();

private slots:
	void countLeftClickStateChanged();
	void displayIndexStateChanged();
	void trackDetonateStateChanged();

	void set_left_click_visibility();
	void set_middle_click_visibility();
	void set_right_click_visibility();
	void set_skill_use_visibility();
	void set_flask_use_visibility();
	void set_detonate_visibility();
	void set_movement_lock();

	void set_display_tracker();
	void set_display_apm();

	void apm_slider_moved(int);

	void toggleLocked();

signals:
	void set_movement_locked(bool);
	void set_display_index(int);
	void set_tracker_visibility(std::wstring, bool);
	void set_show_tracker(bool);
	void set_show_apm(bool);
	void set_apm_timer_window(int);

private:
	Ui::SettingsForm *ui;

	bool movement_locked = true;

	void closeEvent(QCloseEvent*) override;
};
