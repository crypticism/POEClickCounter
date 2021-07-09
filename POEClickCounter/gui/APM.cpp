#include <QTimer>

#include "APM.h"
#include "ui_APM.h"

#include "Manager.h"

APM::APM(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::APM)
{
	ui->setupUi(this);

	// Connect to global events
	connect(&Manager::instance(), &Manager::input_event, this, &APM::increment_count);
	connect(&Manager::instance(), &Manager::reset_session_data, this, &APM::reset_session_data);
	connect(&Manager::instance(), &Manager::movement_lock_change, this, &APM::set_movement_locked);
	connect(&Manager::instance(), &Manager::window_visibility, this, &APM::set_visibility);

	this->t_updateInterface = new QTimer(this);
	connect(t_updateInterface, &QTimer::timeout, this, &APM::update_interface);
	this->t_updateInterface->start(2000);
	
	this->setWindowFlags({ Qt::FramelessWindowHint, Qt::WindowStaysOnTopHint, Qt::SubWindow });
	this->setAttribute(Qt::WA_TranslucentBackground);

	// Set window position based on config values
	json::JsonObject& settings = Data::get_settings();
	double x_pos = settings.GetNamedNumber(APM_X_COORDINATE);
	double y_pos = settings.GetNamedNumber(APM_Y_COORDINATE);
	move(QPoint(int(x_pos), int(y_pos)));

	this->show();
}

APM::~APM()
{
}

void APM::update_interface() {
	while (this->q_apmQueue.size() >= this->n_queueSize) {
		this->q_apmQueue.pop_front();
	}

	int n_actionCount = this->n_count;
	this->n_count = 0;
	this->q_apmQueue.push_back(n_actionCount);

	int n_actionSum = 0;

	for (int i : this->q_apmQueue) {
		n_actionSum += i;
	}

	ui->apm_value->setText(QString::fromStdString(std::to_string(n_actionSum)));
}

void APM::increment_count() {
	this->n_count++;
}

void APM::set_movement_locked(bool b_locked) {
	this->movement_locked = b_locked;
}

void APM::set_visibility(bool b_visible) {
	this->setVisible(b_visible);
}

void APM::reset_session_data() {
	this->q_apmQueue.clear();
	ui->apm_value->setText(QString::fromStdString(std::to_string(0)));
}

void APM::mousePressEvent(QMouseEvent* evt)
{
	this->old_pos = evt->globalPos();
}

void APM::mouseMoveEvent(QMouseEvent* evt)
{
	if (this->movement_locked)
	{
		return;
	}

	const QPoint delta = evt->globalPos() - this->old_pos;
	move(x() + delta.x(), y() + delta.y());

	this->old_pos = evt->globalPos();
	Data::update_settings(APM_X_COORDINATE, json::value(this->x()));
	Data::update_settings(APM_Y_COORDINATE, json::value(this->y()));
}