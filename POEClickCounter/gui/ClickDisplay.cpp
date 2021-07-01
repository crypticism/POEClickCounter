#include "../pch.h"

#include "ClickDisplay.h"
#include "../io/file.h"
#include "../io/ini.h"

std::wstring get_active_window();
double increment_value(std::wstring);

ClickDisplay& ClickDisplay::instance()
{
    static ClickDisplay _instance;
    return _instance;
}

ClickDisplay::ClickDisplay(QWidget *parent)
	: QWidget(parent),
    ui(new Ui::ClickDisplay)
{
    OutputDebugString(L"Here\n");
    ui->setupUi(this);
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hh_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook, hInstance, 0);
	hh_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, hInstance, 0);

    this->data = File::load_data();
    INI::load_ini();

    ui->l_click_value->setText(calculateLabel(getValue(LEFT_CLICK)));
    ui->m_click_value->setText(calculateLabel(getValue(MIDDLE_CLICK)));
    ui->r_click_value->setText(calculateLabel(getValue(RIGHT_CLICK)));
    ui->skill_value->setText(calculateLabel(getValue(SKILL_USE)));
    ui->flask_value->setText(calculateLabel(getValue(FLASK_USE)));
}

ClickDisplay::~ClickDisplay()
{
    File::save_data(this->data);
    UnhookWindowsHookEx(this->hh_mouse_hook);
    UnhookWindowsHookEx(this->hh_keyboard_hook);
}

LRESULT CALLBACK ClickDisplay::mouse_hook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && (
        wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP || wParam == WM_MBUTTONUP
        )) {

        std::wstring title = get_active_window();

        if (title == L"POEClickCounter (Running) - Microsoft Visual Studio") {
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
        std::wstring title = get_active_window();

        if (title == L"POEClickCounter (Running) - Microsoft Visual Studio") {
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