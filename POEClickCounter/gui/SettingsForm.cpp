#include "../pch.h"

#include <QCloseEvent>

#include "Manager.h"
#include "StackedDisplayContainer.h"
#include "SettingsForm.h"
#include "ui_SettingsForm.h"

SettingsForm::SettingsForm(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SettingsForm)
{
	ui->setupUi(this);

	this->setWindowIcon(get_icon());

	json::JsonObject settings = Data::get_settings();

	// Display index of 0 is the index that shows session data, hence this abomination
	ui->display_index->setChecked(!bool(int(settings.GetNamedNumber(DISPLAY_INDEX))));
	ui->display_tracker->setChecked(settings.GetNamedBoolean(DISPLAY_TRACKER));
	ui->display_apm->setChecked(settings.GetNamedBoolean(DISPLAY_APM));
	ui->count_left_click_as_skill->setChecked(settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE));
	ui->track_detonate->setChecked(settings.GetNamedBoolean(TRACK_DETONATE));

	// Initialize active fields
	ui->left_click_visibility->setChecked(Data::is_count_visible(LEFT_CLICK));
	ui->middle_click_visibility->setChecked(Data::is_count_visible(MIDDLE_CLICK));
	ui->right_click_visibility->setChecked(Data::is_count_visible(RIGHT_CLICK));
	ui->skill_use_visibility->setChecked(Data::is_count_visible(SKILL_USE));
	ui->flask_use_visibility->setChecked(Data::is_count_visible(FLASK_USE));
	ui->detonate_visibility->setChecked(Data::is_count_visible(DETONATE));

	// Connect fields to tracker display
	connect(ui->left_click_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_left_click_visibility);
	connect(ui->middle_click_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_middle_click_visibility);
	connect(ui->right_click_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_right_click_visibility);
	connect(ui->skill_use_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_skill_use_visibility);
	connect(ui->flask_use_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_flask_use_visibility);
	connect(ui->detonate_visibility, &QCheckBox::stateChanged, this, &SettingsForm::set_detonate_visibility);
	
	connect(ui->display_index, &QCheckBox::stateChanged, this, &SettingsForm::displayIndexStateChanged);
	connect(ui->count_left_click_as_skill, &QCheckBox::stateChanged, this, &SettingsForm::countLeftClickStateChanged);
	connect(ui->track_detonate, &QCheckBox::stateChanged, this, &SettingsForm::trackDetonateStateChanged);
	connect(ui->movement_locked, &QCheckBox::stateChanged, this, &SettingsForm::set_movement_lock);

	connect(ui->apm_time_window, &QSlider::valueChanged, this, &SettingsForm::apm_slider_moved);

	connect(ui->display_tracker, &QCheckBox::stateChanged, this, &SettingsForm::set_display_tracker);
	connect(ui->display_apm, &QCheckBox::stateChanged, this, &SettingsForm::set_display_apm);
	connect(ui->display_flask_tracker, &QCheckBox::stateChanged, this, &SettingsForm::set_display_flask_tracker);
	connect(ui->display_skill_tracker, &QCheckBox::stateChanged, this, &SettingsForm::set_display_skill_tracker);
}

void SettingsForm::displayIndexStateChanged() {
	int display_index = 1;
	if (ui->display_index->isChecked()) {
		display_index = 0;
	}
	Data::update_session(DISPLAY_INDEX, json::value(display_index));
	emit set_display_index(display_index);
}

void SettingsForm::countLeftClickStateChanged() {
	Data::update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(ui->count_left_click_as_skill->isChecked()));
}

void SettingsForm::trackDetonateStateChanged() {
	Data::update_settings(TRACK_DETONATE, json::value(ui->track_detonate->isChecked()));
}

void SettingsForm::toggleLocked() {
	this->movement_locked = !this->movement_locked;

	if (this->movement_locked) {
		ui->movement_locked->setText(UNLOCK_GUI_STRING);
	}
	else {
		ui->movement_locked->setText(LOCK_GUI_STRING);
	}
}

void SettingsForm::set_left_click_visibility() {
	emit this->set_tracker_visibility(LEFT_CLICK, ui->left_click_visibility->isChecked());
}

void SettingsForm::set_middle_click_visibility() {
	emit this->set_tracker_visibility(MIDDLE_CLICK, ui->middle_click_visibility->isChecked());
}

void SettingsForm::set_right_click_visibility() {
	emit this->set_tracker_visibility(RIGHT_CLICK, ui->right_click_visibility->isChecked());
}

void SettingsForm::set_skill_use_visibility() {
	emit this->set_tracker_visibility(SKILL_USE, ui->skill_use_visibility->isChecked());
}

void SettingsForm::set_flask_use_visibility() {
	emit this->set_tracker_visibility(FLASK_USE, ui->flask_use_visibility->isChecked());
}

void SettingsForm::set_detonate_visibility() {
	emit this->set_tracker_visibility(DETONATE, ui->detonate_visibility->isChecked());
}

void SettingsForm::set_movement_lock() {
	emit this->set_movement_locked(ui->movement_locked->isChecked());
}

void SettingsForm::set_display_tracker() {
	emit this->set_show_tracker(ui->display_tracker->isChecked());
}

void SettingsForm::set_display_apm() {
	emit this->set_show_apm(ui->display_apm->isChecked());
}

void SettingsForm::set_display_flask_tracker() {
	emit this->set_show_flask_tracker(ui->display_flask_tracker->isChecked());
}

void SettingsForm::set_display_skill_tracker() {
	emit this->set_show_skill_tracker(ui->display_skill_tracker->isChecked());
}

void SettingsForm::apm_slider_moved(int value) {
	emit this->set_apm_timer_window(value * 30);
}

void SettingsForm::showSettings() {
	this->show();
}

void SettingsForm::closeEvent(QCloseEvent* event) {
	event->ignore();
	Manager::instance().set_movement_lock(1);
	this->hide();
}

SettingsForm::~SettingsForm()
{
}
