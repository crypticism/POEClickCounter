#pragma once

#include "../pch.h"

#include <QWidget>
#include "ui_Manager.h"

class Manager : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(Manager)

public:
	static Manager &instance();
	Manager(QWidget *parent = Q_NULLPTR);
	~Manager();

	static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam);
	bool is_process_active(DWORD);

private:
	void check_window_visibility();

public slots:
	void set_movement_lock(int);
	void reset_session();
	void refresh_keybinds();
	void set_settings_visible();
	void set_never_show_tracker(bool);
	void set_never_show_apm(bool);

signals:
	void input_event(std::wstring, int, bool);
	void movement_lock_change(bool);
	void reset_session_data();
	void show_settings();
	void tracker_visibility(bool);
	void apm_visibility(bool);

private:
	Ui::Manager ui;

	QTimer *t_checkWindowActive;

	HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;

	bool is_skill_bar_toggled = false;
};
