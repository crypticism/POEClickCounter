
#include "ui_SkillTracker.h"
#include "SkillTracker.h"

SkillTracker::SkillTracker(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SkillTracker)
{
	ui->setupUi(this);

	// Window settings to make the window bordless, always on top and have no taskbar thing
	this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow, Qt::WindowTransparentForInput });
	// Window setting to allow the background to be transparent
	this->setAttribute(Qt::WA_TranslucentBackground);

	ui->bottom_container->setCurrentIndex(0);

	ui->skill1_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_1)));
	ui->skill2_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_2)));
	ui->skill3_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_3)));

	ui->skill4_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_4)));
	ui->skill5_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_5)));
	ui->skill6_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_6)));
	ui->skill7_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_7)));
	ui->skill8_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_8)));

	ui->skill9_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_9)));
	ui->skill10_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_10)));
	ui->skill11_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_11)));
	ui->skill12_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_12)));
	ui->skill13_value->setText(calculateLabel(Data::get_data_specific_skill_value(SKILL_13)));

	// Set window position based on config values
	json::JsonObject& settings = Data::get_settings();
	double x_pos = settings.GetNamedNumber(SKILL_TRACKER_X_COORDINATE);
	double y_pos = settings.GetNamedNumber(SKILL_TRACKER_Y_COORDINATE);
	move(QPoint(int(x_pos), int(y_pos)));

	double height = settings.GetNamedNumber(SKILL_TRACKER_HEIGHT);
	this->set_height(int(height));

	double width = settings.GetNamedNumber(SKILL_TRACKER_WIDTH);
	this->set_width(int(width));
}

SkillTracker::~SkillTracker()
{
}

// Increments the UI and data store when an event is received
void SkillTracker::handle_input_event(std::wstring event_type, int id, bool toggled)
{
	if (event_type != SKILL_USE) {
		return;
	}

	std::wstring skill_id = INI::find_skill_id(id, toggled);
	double n_value = Data::get_data_specific_skill_value(skill_id);
	QString s_value = calculateLabel(n_value);

	if (skill_id == SKILL_1) {
		ui->skill1_value->setText(s_value);
	}
	else if (skill_id == SKILL_2) {
		ui->skill2_value->setText(s_value);
	}
	else if (skill_id == SKILL_3) {
		ui->skill3_value->setText(s_value);
	}
	else if (skill_id == SKILL_4) {
		ui->skill4_value->setText(s_value);
	}
	else if (skill_id == SKILL_5) {
		ui->skill5_value->setText(s_value);
	}
	else if (skill_id == SKILL_6) {
		ui->skill6_value->setText(s_value);
	}
	else if (skill_id == SKILL_7) {
		ui->skill7_value->setText(s_value);
	}
	else if (skill_id == SKILL_8) {
		ui->skill8_value->setText(s_value);
	}
	else if (skill_id == SKILL_9) {
		ui->skill9_value->setText(s_value);
	}
	else if (skill_id == SKILL_10) {
		ui->skill10_value->setText(s_value);
	}
	else if (skill_id == SKILL_11) {
		ui->skill11_value->setText(s_value);
	}
	else if (skill_id == SKILL_12) {
		ui->skill12_value->setText(s_value);
	}
	else if (skill_id == SKILL_13) {
		ui->skill13_value->setText(s_value);
	}
}

void SkillTracker::toggle_display_index(bool b_toggled) {
	ui->bottom_container->setCurrentIndex(int(b_toggled));
}

void SkillTracker::set_movement_locked(bool b_locked)
{
	if (b_locked && b_locked != this->movement_locked) {
		this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow, Qt::WindowTransparentForInput });
	}
	else if (b_locked != this->movement_locked) {
		this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow });
	}
	this->movement_locked = b_locked;
}

void SkillTracker::set_visibility(bool b_visible) {
	this->setVisible(b_visible);
}

void SkillTracker::set_width(int width) {
	this->setFixedWidth(width);
}

void SkillTracker::set_height(int height) {
	this->setFixedHeight(height);
}

void SkillTracker::mousePressEvent(QMouseEvent* evt)
{
	this->old_pos = evt->globalPos();
}

void SkillTracker::mouseMoveEvent(QMouseEvent* evt)
{
	if (this->movement_locked)
	{
		return;
	}

	const QPoint delta = evt->globalPos() - this->old_pos;
	move(x() + delta.x(), y() + delta.y());

	this->old_pos = evt->globalPos();
	Data::update_settings(SKILL_TRACKER_X_COORDINATE, json::value(this->x()));
	Data::update_settings(SKILL_TRACKER_Y_COORDINATE, json::value(this->y()));
}