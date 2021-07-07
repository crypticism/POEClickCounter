#pragma once

#ifndef STACKEDISPLAYCONTAINER_H
#define STACKEDISPLAYCONTAINER_H

#define NOMINMAX

#include "../pch.h"

#include <QWidget>
#include <QtGui>

#include "ui_StackedDisplayContainer.h"

class StackedDisplayContainer : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(StackedDisplayContainer)

public:
	static StackedDisplayContainer& instance();
	explicit StackedDisplayContainer(QWidget *parent = Q_NULLPTR);
	~StackedDisplayContainer();

	static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

	bool getMovementLocked() {
		return movement_locked;
	}

	void setGUIMode(int);

public slots:
	void handleUpdate(std::wstring, std::wstring);
	void isApplicationActive();
	void setIsCheckingActive();
	void resetSessionData();
	void toggleLock();

	void setLeftClickVisibility(int);
	void setMiddleClickVisibility(int);
	void setRightClickVisibility(int);
	void setSkillUseVisibility(int);
	void setFlaskUseVisibility(int);
	void setDetonateVisibility(int);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

signals:
	// arg1 = event type, arg2 = skill_id if applicable
	void dispatchEvent(std::wstring, std::wstring = L"");
	void checkIsApplicationActive();
	void setApplicationActive();

private:
	Ui::StackedDisplayContainer *ui;

	void updateWidth();

	int container_width;

	bool is_skill_bar_toggled = false;

	bool is_checking_whether_application_active = false;
	bool is_application_active = false;

	bool movement_locked = true;
	QPoint old_pos;

	HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;
};

#endif // STACKEDISPLAYCONTAINER_H