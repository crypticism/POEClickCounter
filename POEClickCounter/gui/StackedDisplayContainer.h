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
	void handleUpdate(std::wstring);
	void isApplicationActive();
	void setIsCheckingActive();
	void neverShow();
	void toggleLock();
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

	bool is_checking_whether_application_active = false;
	bool is_application_active = false;

	bool movement_locked = true;
	QPoint old_pos;

	HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;
};

#endif // STACKEDISPLAYCONTAINER_H