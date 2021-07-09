#include <QTimer>

#include "Manager.h"

Manager& Manager::instance()
{
	static Manager _instance;
	return _instance;
}

Manager::Manager(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	// Set up input listeners
	HINSTANCE hInstance = GetModuleHandle(NULL);
	hh_mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, mouse_hook, hInstance, 0);
	hh_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook, hInstance, 0);

    // Create timer that checks if window is open to show/hide GUI automatically
    this->t_checkWindowActive = new QTimer(this);
    connect(t_checkWindowActive, &QTimer::timeout, this, QOverload<>::of(&Manager::check_window_visibility));
    t_checkWindowActive->start(3000);
}

Manager::~Manager()
{
    Data::save_data();
    Data::save_settings();
    UnhookWindowsHookEx(this->hh_mouse_hook);
    UnhookWindowsHookEx(this->hh_keyboard_hook);
}

void Manager::check_window_visibility() {
    // Reset checking value to false, as we can only determine whether it is open
    // not whether it is not open
    is_checking_whether_application_active = false;

    json::JsonObject settings = Data::get_settings();
    if (settings.GetNamedBoolean(NEVER_SHOW_GUI))
    {
        emit instance().window_visibility(false);
        return;
    }

    EnumWindows(*enum_window_proc, NULL);

    if (is_checking_whether_application_active)
    {
        emit instance().window_visibility(true);
    }
    else
    {
        emit instance().window_visibility(false);
    }
}

LRESULT CALLBACK Manager::mouse_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    if (nCode == HC_ACTION && wParam != WM_MOUSEMOVE)
    {
        // Ensure clicks are being ignored if not in the PoE window
        std::wstring title = get_active_window();

        if (title == application)
        {
            bool b_isToggled = instance().is_skill_bar_toggled;

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
                    emit instance().input_event(SKILL_USE, VK_LBUTTON, b_isToggled);
                }
                emit instance().input_event(LEFT_CLICK, VK_LBUTTON, b_isToggled);
                break;
            }
            case WM_RBUTTONUP:
                if (INI::is_skill_code(VK_RBUTTON))
                {
                    emit instance().input_event(SKILL_USE, VK_RBUTTON, b_isToggled);
                }
                emit instance().input_event(RIGHT_CLICK, VK_RBUTTON, b_isToggled);
                break;

            case WM_MBUTTONUP:
                if (INI::is_skill_code(VK_MBUTTON))
                {
                    emit instance().input_event(SKILL_USE, VK_MBUTTON, b_isToggled);
                }
                emit instance().input_event(MIDDLE_CLICK, VK_MBUTTON, b_isToggled);
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
                        emit instance().input_event(SKILL_USE, input, b_isToggled);
                    }
                    else if (INI::is_detonate_code(VK_XBUTTON1) && settings.GetNamedBoolean(TRACK_DETONATE)) {
                        emit instance().input_event(DETONATE, input, b_isToggled);
                    }
                }
                else if (word == XBUTTON2) {
                    if (INI::is_skill_code(VK_XBUTTON2, b_isToggled)) {
                        emit instance().input_event(SKILL_USE, input, b_isToggled);
                    }
                    else if (INI::is_detonate_code(VK_XBUTTON2) && settings.GetNamedBoolean(TRACK_DETONATE)) {
                        emit instance().input_event(DETONATE, input, b_isToggled);
                    }
                }
                break;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK Manager::keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT* hook_info = (KBDLLHOOKSTRUCT*)lParam;
    DWORD input = hook_info->vkCode;

    // These two checks are not localized to the application window, as Path of Exile also doesn't
    // restrict skill bar toggling to inputs over the Path of Exile window
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN && INI::is_keyboard_toggle_code(input)) {
        instance().is_skill_bar_toggled = true;
    }

    if (nCode == HC_ACTION && wParam == WM_KEYUP && INI::is_keyboard_toggle_code(input)) {
        instance().is_skill_bar_toggled = false;
    }

    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {

        // Ensure keypresses are being ignored if the PoE window is not active
        std::wstring title = get_active_window();

        json::JsonObject settings = Data::get_settings();

        if (title == application)
        {
            bool b_isToggled = instance().is_skill_bar_toggled;
            if (INI::is_skill_code(input, b_isToggled))
            {
                emit instance().input_event(SKILL_USE, input, b_isToggled);
            }
            if (INI::is_flask_code(input))
            {
                emit instance().input_event(FLASK_USE, input, b_isToggled);
            }
            if (INI::is_detonate_code(input) && settings.GetNamedBoolean(TRACK_DETONATE))
            {
                emit instance().input_event(DETONATE, input, b_isToggled);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Loops through all windows and checks if they match the defined application name
// If any do, emit an event to note that application is open
BOOL CALLBACK Manager::enum_window_proc(HWND hwnd, LPARAM lParam)
{
    int length = GetWindowTextLength(hwnd) + 1;

    std::wstring buffer(length, '\0');
    LPWSTR title = reinterpret_cast<LPWSTR>(&buffer[0]);
    GetWindowText(hwnd, title, length);

    if (IsWindow(hwnd) && title == application)
    {
        instance().is_checking_whether_application_active = true;
        return TRUE;
    }

    return TRUE;
};

void Manager::set_movement_lock(int locked) {
    emit movement_lock_change(bool(locked));
}

void Manager::set_never_show(int state) {
    Data::update_settings(NEVER_SHOW_GUI, json::value(bool(state)));
    emit instance().window_visibility(bool(state));
}

void Manager::reset_session() {
    emit reset_session_data();
}

void Manager::refresh_keybinds() {
    INI::load_ini();
}