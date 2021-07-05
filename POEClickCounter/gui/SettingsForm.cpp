#include "../pch.h"

#include <QCloseEvent>

#include "../io/data.h"
#include "../utils/utils.h"
#include "StackedDisplayContainer.h"
#include "SettingsForm.h"
#include "ui_SettingsForm.h"

SettingsForm& SettingsForm::instance()
{
	static SettingsForm _instance;
	return _instance;
}

SettingsForm::SettingsForm(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::SettingsForm)
{
	ui->setupUi(this);

	this->setWindowIcon(get_icon());

	json::JsonObject settings = Data::get_settings();

	// Display index of 0 is the index that shows session data, hence this abomination
	ui->display_index->setChecked(!bool(int(settings.GetNamedNumber(DISPLAY_INDEX))));
	ui->never_show->setChecked(settings.GetNamedBoolean(NEVER_SHOW_GUI));
	ui->count_left_click_as_skill->setChecked(settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE));
	
	connect(ui->display_index, &QCheckBox::stateChanged, this, &SettingsForm::displayIndexStateChanged);
	connect(ui->never_show, &QCheckBox::stateChanged, this, &SettingsForm::neverShowStateChanged);
	connect(ui->count_left_click_as_skill, &QCheckBox::stateChanged, this, &SettingsForm::countLeftClickStateChanged);

	connect(ui->movement_locked, &QPushButton::pressed, &StackedDisplayContainer::instance(), &StackedDisplayContainer::toggleLock);
	connect(ui->movement_locked, &QPushButton::pressed, this, &SettingsForm::toggleLocked);

	connect(ui->save_button, &QPushButton::pressed, this, &SettingsForm::saveSettings);
}

void SettingsForm::displayIndexStateChanged() {
	int display_index = 1;
	if (ui->display_index->isChecked()) {
		display_index = 0;
	}
	Data::update_session(DISPLAY_INDEX, json::value(display_index));
	StackedDisplayContainer::instance().setGUIMode(display_index);
}

void SettingsForm::neverShowStateChanged(int state) {
	Data::update_settings(NEVER_SHOW_GUI, json::value(ui->never_show->isChecked()));
}

void SettingsForm::countLeftClickStateChanged() {
	Data::update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(ui->count_left_click_as_skill->isChecked()));
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

void SettingsForm::saveSettings() {
	Data::save_settings();
	this->hide();
}

void SettingsForm::showSettings() {
	this->show();
}

void SettingsForm::closeEvent(QCloseEvent* event) {
	event->ignore();
	this->hide();
}

SettingsForm::~SettingsForm()
{
}
