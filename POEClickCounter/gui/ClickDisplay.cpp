#include "../pch.h"

#include <QPushButton>
#include <QTimer>

#include "ClickDisplay.h"
#include "../io/file.h"
#include "../io/ini.h"

std::wstring get_active_window();
double increment_value(std::wstring);
double getValueFromJson(std::wstring event_type);
QString calculateLabel(double value);

ClickDisplay& ClickDisplay::instance()
{
    static ClickDisplay _instance;
    return _instance;
}

ClickDisplay::ClickDisplay(QWidget *parent)
	: QWidget(parent),
    ui(new Ui::ClickDisplay)
{
    ui->setupUi(this);

    // Set ui state based on saved setting
    json::JsonObject& settings = File::get_settings();
    never_show = settings.GetNamedBoolean(L"never_show");

    // Set up input listeners
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hh_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook, hInstance, 0);
	hh_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, hInstance, 0);

    // Load counts into values
    ui->l_click_value->setText(calculateLabel(getValueFromJson(LEFT_CLICK)));
    ui->m_click_value->setText(calculateLabel(getValueFromJson(MIDDLE_CLICK)));
    ui->r_click_value->setText(calculateLabel(getValueFromJson(RIGHT_CLICK)));
    ui->skill_value->setText(calculateLabel(getValueFromJson(SKILL_USE)));
    ui->flask_value->setText(calculateLabel(getValueFromJson(FLASK_USE)));

    // Create timer that checks if window is open to show/hide GUI automatically
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&ClickDisplay::checkIsActive));
    timer->start(3000);

    // Set window position based on config values
    double x_pos = settings.GetNamedNumber(L"x_pos");
    double y_pos = settings.GetNamedNumber(L"y_pos");
    move(QPoint(int(x_pos), int(y_pos)));
}

ClickDisplay::~ClickDisplay()
{
    File::save_data();
    File::save_settings();
    UnhookWindowsHookEx(this->hh_mouse_hook);
    UnhookWindowsHookEx(this->hh_keyboard_hook);
}

// Loops through all windows and checks if they match the defined application name
// If any do, emit an event to note that application is open
BOOL CALLBACK ClickDisplay::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    int length = GetWindowTextLength(hwnd) + 1;

    std::wstring buffer(length, '\0');
    LPWSTR title = reinterpret_cast<LPWSTR>(&buffer[0]);
    GetWindowText(hwnd, title, length);

    if (IsWindowVisible(hwnd) && title == application) {
        emit instance().setIsActive();
        return TRUE;
    }

    return TRUE;
};

LRESULT CALLBACK ClickDisplay::mouse_hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && (
        wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP
        )) {

        // Ensure clicks are being ignored if not in the PoE window
        std::wstring title = get_active_window();

        if (title == application) {
            // Skills can be bound to mouse buttons
            if (INI::is_skill_code(VK_LBUTTON)) {
                emit instance().handleEvent(SKILL_USE);
            }

            switch (wParam) {
            case WM_LBUTTONUP:
                emit instance().handleEvent(LEFT_CLICK);
                break;
            case WM_RBUTTONUP:
                emit instance().handleEvent(RIGHT_CLICK);
                break;
            case WM_MBUTTONUP:
                emit instance().handleEvent(MIDDLE_CLICK);
                break;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK ClickDisplay::keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && wParam == WM_KEYUP) {

        // Ensure keypresses are being ignored if the PoE window is not active
        std::wstring title = get_active_window();

        if (title == application) {
            KBDLLHOOKSTRUCT* hook_info = (KBDLLHOOKSTRUCT*)lParam;
            DWORD input = hook_info->vkCode;
            if (INI::is_skill_code(input)) {
                emit instance().handleEvent(SKILL_USE);
            }
            if (INI::is_flask_code(input)) {
                emit instance().handleEvent(FLASK_USE);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Find the name of the window that cursor is currently over
// Seems to correctly grab only the top-most window
 std::wstring get_active_window() {
     POINT point;
     GetCursorPos(&point);
     HWND new_hwnd = WindowFromPoint(point);

     int nTitleLength = GetWindowTextLength(new_hwnd) + 1;

     std::wstring s(nTitleLength, '\0');
     LPWSTR title = reinterpret_cast<LPWSTR>(&s[0]);

     GetWindowText(new_hwnd, title, nTitleLength);
     return title;
 }

 // Increments the UI and data store when an event is received
 void ClickDisplay::dispatchUpdate(std::wstring event_type) {
     double nValue = getValueFromJson(event_type) + 1;
     json::JsonObject& data = File::get_data();
     data.SetNamedValue(event_type, json::value(nValue));

     QString sValue = calculateLabel(nValue);

     if (event_type == LEFT_CLICK) {
         ui->l_click_value->setText(sValue);
         return;
     }
     if (event_type == MIDDLE_CLICK) {
         ui->m_click_value->setText(sValue);
         return;
     }
     if (event_type == RIGHT_CLICK) {
         ui->r_click_value->setText(sValue);
         return;
     }
     if (event_type == SKILL_USE) {
         ui->skill_value->setText(sValue);
         return;
     }
     if (event_type == FLASK_USE) {
         ui->flask_value->setText(sValue);
         return;
     }
 };

 // Retrieves a value from the data store based on the event
 // Could go into the file managing other data stores
 double getValueFromJson(std::wstring event_type) {
     double value = 0.0;
     json::JsonObject& data = File::get_data();
     if (json::has(data, event_type, json::JsonValueType::Number)) {
         value = floor(data.GetNamedNumber(event_type));
     }
     return value;
 }

 // Converts a double value into a truncated string with a suffix to save space
 QString calculateLabel(double value) {
     double c = value == 0.0 ? 0.0 : floor(log10(abs(value)) / 3);
     double m = std::min(5.0, c);
     double max = std::max(0.0, m);

     int index = max;

     std::string buffer = std::to_string(int(floor(value)));

     for (int i = 0; i < 3 * (index - 1); i++) {
         buffer.pop_back();
     }

     if (index >= 1) {
         std::string to_insert = ".";
         int pos = int(buffer.length()) - 3;
         buffer.insert(pos, to_insert);
     }

     return QString::fromUtf8(buffer).append(suffixes[index]);
 };