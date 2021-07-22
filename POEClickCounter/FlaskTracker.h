#ifndef FLASKTRACKER_H
#define FLASKTRACKER_H

#define NOMINMAX

#include "pch.h"

#include <QWidget>
#include <QtGui>
#include "ui_FlaskTracker.h"

class FlaskTracker : public QWidget
{
	Q_OBJECT

public:
	FlaskTracker(QWidget *parent = Q_NULLPTR);
	~FlaskTracker();

public slots:
	void handle_input_event(std::wstring, int, bool);
	void set_movement_locked(bool);
	void set_visibility(bool);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);

private:
	Ui::FlaskTracker *ui;

	bool movement_locked = true;
	QPoint old_pos;
};

#endif // FLASKTRACKER_H