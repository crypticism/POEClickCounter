#include "pch.h"

#include <QPushButton>
#include <QTimer>

#include "ui_StackedDisplayContainer.h"
#include "StackedDisplayContainer.h"
#include "../io/data.h"
#include "../io/ini.h"
#include "../utils/utils.h"

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

    // Set ui state based on saved setting
    json::JsonObject &settings = Data::get_settings();
    ui->WidgetContainer->setCurrentIndex(int(settings.GetNamedNumber(DISPLAY_INDEX)));

    // Set up input listeners
    HINSTANCE hInstance = GetModuleHandle(NULL);
    hh_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook, hInstance, 0);
    hh_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, hInstance, 0);

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

    // Create timer that checks if window is open to show/hide GUI automatically
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&StackedDisplayContainer::checkIsApplicationActive));
    timer->start(3000);

    // Set window position based on config values
    double x_pos = settings.GetNamedNumber(GUI_X_COORDINATE);
    double y_pos = settings.GetNamedNumber(GUI_Y_COORDINATE);
    move(QPoint(int(x_pos), int(y_pos)));
}

StackedDisplayContainer::~StackedDisplayContainer()
{
    Data::save_data();
    Data::save_settings();
    UnhookWindowsHookEx(this->hh_mouse_hook);
    UnhookWindowsHookEx(this->hh_keyboard_hook);
}

// Loops through all windows and checks if they match the defined application name
// If any do, emit an event to note that application is open
BOOL CALLBACK StackedDisplayContainer::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    int length = GetWindowTextLength(hwnd) + 1;

    std::wstring buffer(length, '\0');
    LPWSTR title = reinterpret_cast<LPWSTR>(&buffer[0]);
    GetWindowText(hwnd, title, length);

    if (IsWindow(hwnd) && title == application)
    {
        emit instance().setApplicationActive();
        return TRUE;
    }

    return TRUE;
};

LRESULT CALLBACK StackedDisplayContainer::mouse_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && wParam != WM_MOUSEMOVE)
    {
        // Ensure clicks are being ignored if not in the PoE window
        std::wstring title = get_active_window();

        if (title == application)
        {
            bool b_isToggled = StackedDisplayContainer::instance().is_skill_bar_toggled;
            
            switch (wParam)
            {
            case WM_LBUTTONUP:
            {
                // This is probably an inefficient way to do this
                json::JsonObject settings = Data::get_settings();
                bool count_left_click_as_skill = settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE);

                // Skills can be bound to mouse buttons, but POE uses the virtual keycodes for them
                if (INI::is_skill_code(VK_LBUTTON) && count_left_click_as_skill)
                {
                    emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(VK_LBUTTON, b_isToggled));
                }
                emit instance().dispatchEvent(LEFT_CLICK);
                break;
            }
            case WM_RBUTTONUP:
                if (INI::is_skill_code(VK_RBUTTON))
                {
                    emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(VK_RBUTTON, b_isToggled));
                }
                emit instance().dispatchEvent(RIGHT_CLICK);
                break;

            case WM_MBUTTONUP:
                if (INI::is_skill_code(VK_MBUTTON))
                {
                    emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(VK_MBUTTON, b_isToggled));
                }
                emit instance().dispatchEvent(MIDDLE_CLICK);
                break;
            case WM_XBUTTONUP:
                MSLLHOOKSTRUCT* hook_info = (MSLLHOOKSTRUCT*)lParam;
                DWORD input = hook_info->mouseData;

                // mouseData stores the actual input in a high-order/low-order pair
                // shifting bits allows us to grab the actual value
                auto word = input >> 16;

                json::JsonObject settings = Data::get_settings();

                // XBUTTON1 and XBUTTON2 map to 1 and 2 respectively, this means we need to use VK_XBUTTON1 and VK_XBUTTON2
                // as those are the actual values used in the config file
                if (word == XBUTTON1) {
                    if (INI::is_skill_code(VK_XBUTTON1, b_isToggled)) {
                        emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(VK_XBUTTON1, b_isToggled));
                    }
                    else if (INI::is_detonate_code(VK_XBUTTON1) && settings.GetNamedBoolean(TRACK_DETONATE)) {
                        emit instance().dispatchEvent(DETONATE);
                    }
                }
                else if (word == XBUTTON2) {
                    if (INI::is_skill_code(VK_XBUTTON2, b_isToggled)) {
                        emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(VK_XBUTTON2, b_isToggled));
                    }
                    else if (INI::is_detonate_code(VK_XBUTTON2) && settings.GetNamedBoolean(TRACK_DETONATE)) {
                        emit instance().dispatchEvent(DETONATE);
                    }
                }
                break;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK StackedDisplayContainer::keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* hook_info = (KBDLLHOOKSTRUCT*)lParam;
    DWORD input = hook_info->vkCode;

    // These two checks are not localized to the application window, as Path of Exile also doesn't
    // restrict skill bar toggling to inputs over the Path of Exile window
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN && INI::is_keyboard_toggle_code(input)) {
        StackedDisplayContainer::instance().is_skill_bar_toggled = true;
    }

    if (nCode == HC_ACTION && wParam == WM_KEYUP && INI::is_keyboard_toggle_code(input)) {
        StackedDisplayContainer::instance().is_skill_bar_toggled = false;
    }

    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {

        // Ensure keypresses are being ignored if the PoE window is not active
        std::wstring title = get_active_window();

        json::JsonObject settings = Data::get_settings();

        if (title == application)
        {
            bool b_isToggled = StackedDisplayContainer::instance().is_skill_bar_toggled;
            if (INI::is_skill_code(input, b_isToggled))
            {
                emit instance().dispatchEvent(SKILL_USE, INI::find_skill_id(input, b_isToggled));
            }
            if (INI::is_flask_code(input))
            {
                emit instance().dispatchEvent(FLASK_USE, INI::find_flask_id(input));
            }
            if (INI::is_detonate_code(input) && settings.GetNamedBoolean(TRACK_DETONATE))
            {
                emit instance().dispatchEvent(DETONATE);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Increments the UI and data store when an event is received
void StackedDisplayContainer::handleUpdate(std::wstring event_type, std::wstring skill_id)
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

        int n_skillCount = Data::get_data_specific_skill_value(skill_id);
        Data::update_data_specific_skill_value(skill_id, json::value(n_skillCount + 1));

        return;
    }
    if (event_type == FLASK_USE)
    {
        ui->flask_value->setText(sValue);
        ui->flask_value_2->setText(sValue);
        ui->flask_session_value->setText(sSessionValue);

        int n_flaskCount = Data::get_data_specific_flask_value(skill_id);
        Data::update_data_specific_flask_value(skill_id, json::value(n_flaskCount + 1));

        return;
    }
    if (event_type == DETONATE) {
        ui->detonate_value->setText(sValue);
        ui->detonate_value_2->setText(sValue);
        ui->detonate_session_value->setText(sSessionValue);
    }
};

// Check whether the application is open or not
void StackedDisplayContainer::isApplicationActive()
{
    // Reset checking value to false, as we can only determine whether it is open
    // not whether it is not open
    is_checking_whether_application_active = false;

    json::JsonObject settings = Data::get_settings();
    if (settings.GetNamedBoolean(NEVER_SHOW_GUI))
    {
        this->hide();
        return;
    }

    EnumWindows(*EnumWindowsProc, NULL);

    if (is_checking_whether_application_active)
    {
        this->show();
    }
    else
    {
        this->hide();
    }

    is_application_active = is_checking_whether_application_active;
}

void StackedDisplayContainer::setIsCheckingActive()
{
    is_checking_whether_application_active = true;
}

void StackedDisplayContainer::setGUIMode(int display_index)
{
    ui->WidgetContainer->setCurrentIndex(display_index);
}

void StackedDisplayContainer::setLeftClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(LEFT_CLICK, json::value(active));

    ui->LMBContainer->setVisible(active);
    ui->LMBContainer_2->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::setMiddleClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(MIDDLE_CLICK, json::value(active));

    ui->MMBContainer->setVisible(active);
    ui->MMBContainer_2->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::setRightClickVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(RIGHT_CLICK, json::value(active));

    ui->RMBContainer->setVisible(active);
    ui->RMBContainer_2->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::setSkillUseVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(SKILL_USE, json::value(active));

    // I'm not sure why this is _2 and _3
    ui->SkillContainer_2->setVisible(active);
    ui->SkillContainer_3->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::setFlaskUseVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(FLASK_USE, json::value(active));

    // I'm not sure on this one either
    ui->FlaskContainer_2->setVisible(active);
    ui->FlaskContainer_3->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::setDetonateVisibility(int state) {
    bool active(state);
    Data::set_count_visibility(DETONATE, json::value(active));

    ui->DetonateContainer->setVisible(active);
    ui->DetonateContainer_2->setVisible(active);

    updateWidth();
}

void StackedDisplayContainer::updateWidth() {
    int n_visibleCounts = get_number_visible_counts();
    
    int n_widthOfContainers = this->container_width * n_visibleCounts;
    int n_widthofPadding = std::min(0, (n_visibleCounts - 1)) * 6;

    this->setFixedWidth(n_widthOfContainers + n_widthofPadding);
}

void StackedDisplayContainer::toggleLock()
{
    movement_locked = !movement_locked;
}

void StackedDisplayContainer::mousePressEvent(QMouseEvent *evt)
{
    old_pos = evt->globalPos();
}

void StackedDisplayContainer::mouseMoveEvent(QMouseEvent *evt)
{
    if (movement_locked)
    {
        return;
    }

    const QPoint delta = evt->globalPos() - old_pos;
    move(x() + delta.x(), y() + delta.y());

    old_pos = evt->globalPos();
    Data::update_settings(GUI_X_COORDINATE, json::value(this->x()));
    Data::update_settings(GUI_Y_COORDINATE, json::value(this->y()));
}

void StackedDisplayContainer::resetSessionData()
{
    Data::reset_session_data();
    ui->lmb_session_value->setText(QString::fromStdString("0"));
    ui->mmb_session_value->setText(QString::fromStdString("0"));
    ui->rmb_session_value->setText(QString::fromStdString("0"));
    ui->skill_session_value->setText(QString::fromStdString("0"));
    ui->flask_session_value->setText(QString::fromStdString("0"));
    ui->detonate_session_value->setText(QString::fromStdString("0"));
}