#include "pch.h"

#include <QPushButton>
#include <QTimer>

#include "ui_StackedDisplayContainer.h"
#include "StackedDisplayContainer.h"
#include "../io/data.h"
#include "../io/ini.h"
#include "../utils/utils.h"

StackedDisplayContainer& StackedDisplayContainer::instance()
{
	static StackedDisplayContainer _instance;
	return _instance;
}

StackedDisplayContainer::StackedDisplayContainer(QWidget *parent)
	: QWidget(parent),
	ui(new Ui::StackedDisplayContainer)
{
	ui->setupUi(this);

    // Set ui state based on saved setting
    json::JsonObject& settings = Data::get_settings();
    never_show = settings.GetNamedBoolean(NEVER_SHOW_GUI);
    display_index = int(settings.GetNamedNumber(DISPLAY_INDEX));
    ui->WidgetContainer->setCurrentIndex(display_index);

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

    // Create timer that checks if window is open to show/hide GUI automatically
    QTimer* timer = new QTimer(this);
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

    if (IsWindow(hwnd) && title == application) {
        emit instance().setApplicationActive();
        return TRUE;
    }

    return TRUE;
};

LRESULT CALLBACK StackedDisplayContainer::mouse_hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && (
        wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP
        )) {
        // Ensure clicks are being ignored if not in the PoE window
        std::wstring title = get_active_window();

        if (title == application) {
            switch (wParam) {
            case WM_LBUTTONUP:
            {
                // This is probably an inefficient way to do this
                json::JsonObject settings = Data::get_settings();
                bool count_left_click_as_skill = settings.GetNamedBoolean(COUNT_LEFT_CLICK_AS_SKILL_USE);

                // Skills can be bound to mouse buttons, but POE uses the virtual keycodes for them
                if (INI::is_skill_code(VK_LBUTTON) && count_left_click_as_skill) {
                    emit instance().dispatchEvent(SKILL_USE);
                }
                emit instance().dispatchEvent(LEFT_CLICK);
                break;
            }
            case WM_RBUTTONUP:
                if (INI::is_skill_code(VK_RBUTTON)) {
                    emit instance().dispatchEvent(SKILL_USE);
                }
                emit instance().dispatchEvent(RIGHT_CLICK);
                break;

            case WM_MBUTTONUP:
                if (INI::is_skill_code(VK_MBUTTON)) {
                    emit instance().dispatchEvent(SKILL_USE);
                }
                emit instance().dispatchEvent(MIDDLE_CLICK);
                break;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK StackedDisplayContainer::keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && wParam == WM_KEYUP) {

        // Ensure keypresses are being ignored if the PoE window is not active
        std::wstring title = get_active_window();

        if (title == application) {
            KBDLLHOOKSTRUCT* hook_info = (KBDLLHOOKSTRUCT*)lParam;
            DWORD input = hook_info->vkCode;
            if (INI::is_skill_code(input)) {
                emit instance().dispatchEvent(SKILL_USE);
            }
            if (INI::is_flask_code(input)) {
                emit instance().dispatchEvent(FLASK_USE);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Increments the UI and data store when an event is received
void StackedDisplayContainer::handleUpdate(std::wstring event_type) {
    double nValue = Data::get_data_value(event_type) + 1;
    double nSessionValue = Data::get_session_value(event_type) + 1;
    
    Data::update_data(event_type, json::value(nValue));
    Data::update_session(event_type, json::value(nSessionValue));

    QString sValue = calculateLabel(nValue);
    QString sSessionValue = QString::fromStdString("+") + calculateLabel(nSessionValue);

    if (event_type == LEFT_CLICK) {
        ui->l_click_value->setText(sValue);
        ui->l_click_value_2->setText(sValue);
        ui->lmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == MIDDLE_CLICK) {
        ui->m_click_value->setText(sValue);
        ui->m_click_value_2->setText(sValue);
        ui->mmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == RIGHT_CLICK) {
        ui->r_click_value->setText(sValue);
        ui->r_click_value_2->setText(sValue);
        ui->rmb_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == SKILL_USE) {
        ui->skill_value->setText(sValue);
        ui->skill_value_2->setText(sValue);
        ui->skill_session_value->setText(sSessionValue);
        return;
    }
    if (event_type == FLASK_USE) {
        ui->flask_value->setText(sValue);
        ui->flask_value_2->setText(sValue);
        ui->flask_session_value->setText(sSessionValue);
        return;
    }
};

// Check whether the application is open or not
void StackedDisplayContainer::isApplicationActive() {
    // Reset checking value to false, as we can only determine whether it is open
    // not whether it is not open
    is_checking_whether_application_active = false;

    if (never_show) {
        return;
    }

    EnumWindows(*EnumWindowsProc, NULL);

    if (is_checking_whether_application_active) {
        this->show();
    }
    else {
        this->hide();
    }

    is_application_active = is_checking_whether_application_active;
}

void StackedDisplayContainer::setIsCheckingActive() {
    is_checking_whether_application_active = true;
}

void StackedDisplayContainer::neverShow() {
    never_show = !never_show;

    if (never_show) {
        this->hide();
    }
}

void StackedDisplayContainer::toggleLock() {
    movement_locked = !movement_locked;
}

void StackedDisplayContainer::toggleGUIMode() {
    if (display_index) {
        display_index = 0;
    }
    else {
        display_index = 1;
    }
    ui->WidgetContainer->setCurrentIndex(display_index);
    Data::update_settings(DISPLAY_INDEX, json::value(double(display_index)));
}

void StackedDisplayContainer::mousePressEvent(QMouseEvent* evt) {
    old_pos = evt->globalPos();
}

void StackedDisplayContainer::mouseMoveEvent(QMouseEvent* evt)
{
    if (movement_locked) {
        return;
    }

    const QPoint delta = evt->globalPos() - old_pos;
    move(x() + delta.x(), y() + delta.y());

    old_pos = evt->globalPos();
    Data::update_settings(GUI_X_COORDINATE, json::value(this->x()));
    Data::update_settings(GUI_Y_COORDINATE, json::value(this->y()));
}

void StackedDisplayContainer::resetSessionData() {
    Data::reset_session_data();
    ui->lmb_session_value->setText(QString::fromStdString("0"));
    ui->mmb_session_value->setText(QString::fromStdString("0"));
    ui->rmb_session_value->setText(QString::fromStdString("0"));
    ui->skill_session_value->setText(QString::fromStdString("0"));
    ui->flask_session_value->setText(QString::fromStdString("0"));
}