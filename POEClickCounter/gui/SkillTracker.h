#pragma once

#ifndef SKILLTRACKER_H
#define SKILLTRACKER_H

#include "../pch.h"

#include <QWidget>
#include <QTGui>

#include "ui_SkillTracker.h"

class SkillTracker : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(SkillTracker)
public:
	explicit SkillTracker(QWidget *parent = Q_NULLPTR);
	~SkillTracker();

public slots:
	void handle_input_event(std::wstring, int, bool);
	void set_movement_locked(bool);
	void set_visibility(bool);
	void toggle_display_index(bool);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

private:
	Ui::SkillTracker *ui;

	bool movement_locked = true;
	QPoint old_pos;
};

#endif // SKILLTRACKER_H