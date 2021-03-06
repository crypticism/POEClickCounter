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
	explicit StackedDisplayContainer(QWidget *parent = Q_NULLPTR);
	~StackedDisplayContainer();

	bool getMovementLocked() {
		return movement_locked;
	}

public slots:
	void handle_input_event(std::wstring, int, bool);
	void set_movement_locked(bool);
	void set_visibility(bool);
	void reset_session_data();
	void set_gui_mode(int);
	void set_tracker_visibility(std::wstring, bool);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

private:
	Ui::StackedDisplayContainer *ui;

	void update_width();

	int container_width;

	bool movement_locked = true;
	QPoint old_pos;
};

#endif // STACKEDISPLAYCONTAINER_H