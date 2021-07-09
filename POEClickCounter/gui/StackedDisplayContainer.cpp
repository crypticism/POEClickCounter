#include <QPushButton>

#include "ui_StackedDisplayContainer.h"
#include "StackedDisplayContainer.h"
#include "Manager.h"

StackedDisplayContainer &StackedDisplayContainer::instance()
{
    static StackedDisplayContainer _instance;
    return _instance;
}

StackedDisplayContainer::StackedDisplayContainer(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::StackedDisplayContainer)
{
    ui->setupUi(this);

    this->container_width = ui->LMBContainer->width();

    // Connect to global events
    connect(&Manager::instance(), &Manager::input_event, this, &StackedDisplayContainer::handle_input_event);
    connect(&Manager::instance(), &Manager::reset_session_data, this, &StackedDisplayContainer::reset_session_data);
    connect(&Manager::instance(), &Manager::movement_lock_change, this, &StackedDisplayContainer::set_movement_locked);
    connect(&Manager::instance(), &Manager::window_visibility, this, &StackedDisplayContainer::set_visibility);

    // Set ui state based on saved setting
    json::JsonObject &settings = Data::get_settings();
    ui->WidgetContainer->setCurrentIndex(int(settings.GetNamedNumber(DISPLAY_INDEX)));

    // Load counts into values
    ui->l_click_value->setText(calculateLabel(Data::get_data_value(LEFT_CLICK)));
    ui->l_click_value_2->setText(calculateLabel(Data::get_data_value(LEFT_CLICK)));
    ui->lmb_session_value->setText(QString::fromStdString("0"));

    ui->m_click_value->setText(calculateLabel(Data::get_data_value(MIDDLE_CLICK)));
    ui->m_click_value_2->setText(calculateLabel(Data::get_data_value(MIDDLE_CLICK)));
    ui->mmb_session_value->setText(QString::fromStdString("0"));

    ui->r_click_value->setText(calculateLabel(Data::get_data_value(RIGHT_CLICK)));
    ui->r_click_value_2->setText(calculateLabel(Data::get_data_value(RIGHT_CLICK)));
    ui->rmb_session_value->setText(QString::fromStdString("0"));

    ui->skill_value->setText(calculateLabel(Data::get_data_value(SKILL_USE)));
    ui->skill_value_2->setText(calculateLabel(Data::get_data_value(SKILL_USE)));
    ui->skill_session_value->setText(QString::fromStdString("0"));

    ui->flask_value->setText(calculateLabel(Data::get_data_value(FLASK_USE)));
    ui->flask_value_2->setText(calculateLabel(Data::get_data_value(FLASK_USE)));
    ui->flask_session_value->setText(QString::fromStdString("0"));

    ui->detonate_value->setText(calculateLabel(Data::get_data_value(DETONATE)));
    ui->detonate_value_2->setText(calculateLabel(Data::get_data_value(DETONATE)));
    ui->detonate_session_value->setText(QString::fromStdString("0"));

    // Set container visibilities
    setLeftClickVisibility(int(Data::is_count_visible(LEFT_CLICK)));
    setMiddleClickVisibility(int(Data::is_count_visible(MIDDLE_CLICK)));
    setRightClickVisibility(int(Data::is_count_visible(RIGHT_CLICK)));
    setSkillUseVisibility(int(Data::is_count_visible(SKILL_USE)));
    setFlaskUseVisibility(int(Data::is_count_visible(FLASK_USE)));
    setDetonateVisibility(int(Data::is_count_visible(DETONATE)));

    // Set window position based on config values
    double x_pos = settings.GetNamedNumber(TRACKER_X_COORDINATE);
    double y_pos = settings.GetNamedNumber(TRACKER_Y_COORDINATE);
    move(QPoint(int(x_pos), int(y_pos)));

    this->update_width();
}

StackedDisplayContainer::~StackedDisplayContainer()
{
}

// Increments the UI and data store when an event is received
void StackedDisplayContainer::handle_input_event(std::wstring event_type, int id, bool toggled)
{
    double nValue = Data::get_data_value(event_type) + 1;
    double nSessionValue = Data::get_session_value(event_type) + 1;

    Data::update_data(event_type, json::value(nValue));
    Data::update_session(event_type, json::value(nSessionValue));

    QString sValue = calculateLabel(nValue);
    QString sSessionValue = QString::fromStdString("+") + calculateLabel(nSessionValue);

    if (event_type == LEFT_CLICK)
    {
        ui->l_click_value->setText(sValue);
        ui->l_click_value_2->setText(sValue);
        ui->lmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == MIDDLE_CLICK)
    {
        ui->m_click_value->setText(sValue);
        ui->m_click_value_2->setText(sValue);
        ui->mmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == RIGHT_CLICK)
    {
        ui->r_click_value->setText(sValue);
        ui->r_click_value_2->setText(sValue);
        ui->rmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == SKILL_USE)
    {
        ui->skill_value->setText(sValue);
        ui->skill_value_2->setText(sValue);
        ui->skill_session_value->setText(sSessionValue);

        std::wstring skill_id = INI::find_skill_id(id, toggled);

        int n_skillCount = Data::get_data_specific_skill_value(skill_id);
        Data::update_data_specific_skill_value(skill_id, json::value(n_skillCount + 1));

        return;
    }
    if (event_type == FLASK_USE)
    {
        ui->flask_value->setText(sValue);
        ui->flask_value_2->setText(sValue);
        ui->flask_session_value->setText(sSessionValue);

        std::wstring flask_id = INI::find_skill_id(id, toggled);

        int n_flaskCount = Data::get_data_specific_flask_value(flask_id);
        Data::update_data_specific_flask_value(flask_id, json::value(n_flaskCount + 1));

        return;
    }
    if (event_type == DETONATE) {
        ui->detonate_value->setText(sValue);
        ui->detonate_value_2->setText(sValue);
        ui->detonate_session_value->setText(sSessionValue);
    }
};


void StackedDisplayContainer::setGUIMode(int display_index)
{
    ui->WidgetContainer->setCurrentIndex(display_index);
}

void StackedDisplayContainer::setLeftClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(LEFT_CLICK, json::value(active));

    ui->LMBContainer->setVisible(active);
    ui->LMBContainer_2->setVisible(active);

    update_width();
}

void StackedDisplayContainer::setMiddleClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(MIDDLE_CLICK, json::value(active));

    ui->MMBContainer->setVisible(active);
    ui->MMBContainer_2->setVisible(active);

    this->update_width();
}

void StackedDisplayContainer::setRightClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(RIGHT_CLICK, json::value(active));

    ui->RMBContainer->setVisible(active);
    ui->RMBContainer_2->setVisible(active);

    this->update_width();
}

void StackedDisplayContainer::setSkillUseVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(SKILL_USE, json::value(active));

    // I'm not sure why this is _2 and _3
    ui->SkillContainer_2->setVisible(active);
    ui->SkillContainer_3->setVisible(active);

    this->update_width();
}

void StackedDisplayContainer::setFlaskUseVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(FLASK_USE, json::value(active));

    // I'm not sure on this one either
    ui->FlaskContainer_2->setVisible(active);
    ui->FlaskContainer_3->setVisible(active);

    this->update_width();
}

void StackedDisplayContainer::setDetonateVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(DETONATE, json::value(active));

    ui->DetonateContainer->setVisible(active);
    ui->DetonateContainer_2->setVisible(active);

    this->update_width();
}

void StackedDisplayContainer::update_width() {
    int n_visibleCounts = get_number_visible_counts();
    
    int n_widthOfContainers = this->container_width * n_visibleCounts;
    int n_widthofPadding = std::min(0, (n_visibleCounts - 1)) * 6;

    this->setFixedWidth(n_widthOfContainers + n_widthofPadding);
}

void StackedDisplayContainer::set_movement_locked(bool b_locked)
{
    this->movement_locked = b_locked;
}

void StackedDisplayContainer::set_visibility(bool b_visible) {
    this->setVisible(b_visible);
}

void StackedDisplayContainer::mousePressEvent(QMouseEvent *evt)
{
    this->old_pos = evt->globalPos();
}

void StackedDisplayContainer::mouseMoveEvent(QMouseEvent *evt)
{
    if (this->movement_locked)
    {
        return;
    }

    const QPoint delta = evt->globalPos() - this->old_pos;
    move(x() + delta.x(), y() + delta.y());

    this->old_pos = evt->globalPos();
    Data::update_settings(TRACKER_X_COORDINATE, json::value(this->x()));
    Data::update_settings(TRACKER_Y_COORDINATE, json::value(this->y()));
}

void StackedDisplayContainer::reset_session_data()
{
    Data::reset_session_data();
    ui->lmb_session_value->setText(QString::fromStdString("0"));
    ui->mmb_session_value->setText(QString::fromStdString("0"));
    ui->rmb_session_value->setText(QString::fromStdString("0"));
    ui->skill_session_value->setText(QString::fromStdString("0"));
    ui->flask_session_value->setText(QString::fromStdString("0"));
    ui->detonate_session_value->setText(QString::fromStdString("0"));
}