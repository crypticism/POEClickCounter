#pragma once

#ifndef STACKEDISPLAYCONTAINER_H
#define STACKEDISPLAYCONTAINER_H

#define NOMINMAX

#include "../pch.h"

#include <QWidget>
#include <QtGui>

#include "ui_StackedDisplayContainer.h"

const std::wstring LEFT_CLICK = L"left_click";
const std::wstring MIDDLE_CLICK = L"middle_click";
const std::wstring RIGHT_CLICK = L"right_click";
const std::wstring SKILL_USE = L"skill";
const std::wstring FLASK_USE = L"flask";

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

public slots:
	void handleUpdate(std::wstring);
	void isApplicationActive();
	void setIsCheckingActive();
	void neverShow();
	void toggleLock();
	void toggleGUIMode();
	void resetSessionData();

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

signals:
	void dispatchEvent(std::wstring);
	void checkIsApplicationActive();
	void setApplicationActive();

private:
	Ui::StackedDisplayContainer *ui;

	int display_index = 1;

	bool never_show = false;

	bool is_checking_whether_application_active = false;
	bool is_application_active = false;

	bool movement_locked = true;
	QPoint old_pos;

	HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;
};

#endif // STACKEDISPLAYCONTAINER_H