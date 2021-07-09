#pragma once

#include "../pch.h"

#include <QWidget>
#include "ui_Manager.h"

class Manager : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(Manager)

public:
	static Manager& instance();
	Manager(QWidget *parent = Q_NULLPTR);
	~Manager();

	static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK enum_window_proc(HWND hwnd, LPARAM lParam);

private:
	void check_window_visibility();

public slots:
	void set_movement_lock(int);
	void set_never_show(int);
	void reset_session();
	void refresh_keybinds();

signals:
	void input_event(std::wstring, int, bool);
	void window_visibility(bool);
	void movement_lock_change(bool);
	void reset_session_data();

private:
	Ui::Manager ui;

	bool is_checking_whether_application_active = false;

	QTimer* t_checkWindowActive;

	HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;

	bool is_skill_bar_toggled = false;
};
