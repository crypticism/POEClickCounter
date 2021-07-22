#include "ui_FlaskTracker.h"
#include "FlaskTracker.h"
#include "Manager.h"


FlaskTracker::FlaskTracker(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::FlaskTracker)
{
	ui->setupUi(this);

	// Window settings to make the window bordless, always on top and have no taskbar thing
	this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow, Qt::WindowTransparentForInput });
	// Window setting to allow the background to be transparent
	this->setAttribute(Qt::WA_TranslucentBackground);

	ui->flask_1_value->setText(calculateLabel(Data::get_data_specific_flask_value(FLASK_1)));
	ui->flask_2_value->setText(calculateLabel(Data::get_data_specific_flask_value(FLASK_2)));
	ui->flask_3_value->setText(calculateLabel(Data::get_data_specific_flask_value(FLASK_3)));
	ui->flask_4_value->setText(calculateLabel(Data::get_data_specific_flask_value(FLASK_4)));
	ui->flask_5_value->setText(calculateLabel(Data::get_data_specific_flask_value(FLASK_5)));

	json::JsonObject& settings = Data::get_settings();
	double x_pos = settings.GetNamedNumber(FLASK_TRACKER_X_COORDINATE);
	double y_pos = settings.GetNamedNumber(FLASK_TRACKER_Y_COORDINATE);
	move(QPoint(int(x_pos), int(y_pos)));

	double width = settings.GetNamedNumber(FLASK_TRACKER_WIDTH);
	this->set_width(int(width));
}

FlaskTracker::~FlaskTracker()
{
}

void FlaskTracker::handle_input_event(std::wstring event_type, int id, bool toggled) {
	if (event_type != FLASK_USE) {
		return;
	}

	std::wstring flask_id = INI::find_flask_id(id);
	double n_value = Data::get_data_specific_flask_value(flask_id);
	QString s_value = calculateLabel(n_value);
	
	if (flask_id == FLASK_1) {
		ui->flask_1_value->setText(s_value);
	}
	else if (flask_id == FLASK_2) {
		ui->flask_2_value->setText(s_value);
	}
	else if (flask_id == FLASK_3) {
		ui->flask_3_value->setText(s_value);
	}
	else if (flask_id == FLASK_4) {
		ui->flask_4_value->setText(s_value);
	}
	else if (flask_id == FLASK_5) {
		ui->flask_5_value->setText(s_value);
	}
}

void FlaskTracker::set_movement_locked(bool b_locked)
{
	if (b_locked && b_locked != this->movement_locked) {
		this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow, Qt::WindowTransparentForInput });
	}
	else if (b_locked != this->movement_locked) {
		this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow });
	}
	this->movement_locked = b_locked;
}

void FlaskTracker::set_visibility(bool b_visible) {
	this->setVisible(b_visible);
}

void FlaskTracker::set_width(int width) {
	this->setFixedWidth(width);
}

void FlaskTracker::mousePressEvent(QMouseEvent* evt)
{
	this->old_pos = evt->globalPos();
}

void FlaskTracker::mouseMoveEvent(QMouseEvent* evt)
{
	if (this->movement_locked)
	{
		return;
	}

	const QPoint delta = evt->globalPos() - this->old_pos;
	move(x() + delta.x(), y() + delta.y());

	this->old_pos = evt->globalPos();
	Data::update_settings(FLASK_TRACKER_X_COORDINATE, json::value(this->x()));
	Data::update_settings(FLASK_TRACKER_Y_COORDINATE, json::value(this->y()));
}