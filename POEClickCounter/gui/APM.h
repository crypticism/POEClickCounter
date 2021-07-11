#pragma once

#include "../pch.h"

#include <QWidget>
#include <QtGui>
#include <deque>

#include "ui_APM.h"

class APM : public QWidget
{
	Q_OBJECT

public:
	APM(QWidget *parent = Q_NULLPTR);
	~APM();

public slots:
	void increment_count();
	void update_interface();

	void set_movement_locked(bool);
	void set_visibility(bool);
	void reset_session_data();
	void set_timer_window(int);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

private:
	Ui::APM *ui;

	bool movement_locked = true;
	QPoint old_pos;

	QTimer* t_updateInterface;
	std::deque<int> q_apmQueue;

	int n_count = 0;
	int n_updateTime = 2;
	int n_timerWindow = 30;
	int n_queueSize = 15;
};
