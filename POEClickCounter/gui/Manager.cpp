#include <QTimer>

#include "Manager.h"
#include "APM.h"
#include "StackedDisplayContainer.h"
#include "SettingsForm.h"

#include <Psapi.h>

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

    APM* apm = new APM;
    connect(this, &Manager::input_event, apm, &APM::increment_count);
    connect(this, &Manager::reset_session_data, apm, &APM::reset_session_data);
    connect(this, &Manager::movement_lock_change, apm, &APM::set_movement_locked);
    connect(this, &Manager::apm_visibility, apm, &APM::set_visibility);

    StackedDisplayContainer* sdc = new StackedDisplayContainer;
    connect(this, &Manager::input_event, sdc, &StackedDisplayContainer::handle_input_event);
    connect(this, &Manager::reset_session_data, sdc, &StackedDisplayContainer::reset_session_data);
    connect(this, &Manager::movement_lock_change, sdc, &StackedDisplayContainer::set_movement_locked);
    connect(this, &Manager::tracker_visibility, sdc, &StackedDisplayContainer::set_visibility);

    SettingsForm* sf = new SettingsForm;
    connect(sf, &SettingsForm::set_movement_locked, this, &Manager::set_movement_lock);
    connect(sf, &SettingsForm::set_show_tracker, this, &Manager::set_never_show_tracker);
    connect(sf, &SettingsForm::set_show_apm, this, &Manager::set_never_show_apm);
    connect(sf, &SettingsForm::set_display_index, sdc, &StackedDisplayContainer::set_gui_mode);
    connect(sf, &SettingsForm::set_tracker_visibility, sdc, &StackedDisplayContainer::set_tracker_visibility);
    connect(sf, &SettingsForm::set_apm_timer_window, apm, &APM::set_timer_window);

    connect(this, &Manager::show_settings, sf, &SettingsForm::showSettings);
}

Manager::~Manager()
{
    Data::save_data();
    Data::save_settings();
    UnhookWindowsHookEx(this->hh_mouse_hook);
    UnhookWindowsHookEx(this->hh_keyboard_hook);
}

void Manager::check_window_visibility() {
    bool process_active = false;

    json::JsonObject settings = Data::get_settings();
    if (!settings.GetNamedBoolean(DISPLAY_TRACKER))
    {
        emit instance().tracker_visibility(false);
        return;
    }

    if (!settings.GetNamedBoolean(DISPLAY_APM))
    {
        emit instance().apm_visibility(false);
        return;
    }

    // List all process names
    DWORD a_processes[1024], cb_needed;
    EnumProcesses(a_processes, sizeof(a_processes), &cb_needed);
    DWORD n_processes = cb_needed / sizeof(DWORD);

    // Check each process to see if it is a Path of Exile process
    for (int i = 0; i < n_processes; i++) {
        if (a_processes[i] != 0) {
            if (this->is_process_active(a_processes[i])) {
                process_active = true;
                break;
            }
        }
    }

    emit instance().tracker_visibility(process_active);
    emit instance().apm_visibility(process_active);
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

bool Manager::is_process_active(DWORD process_id) {
    WCHAR s_processName[MAX_PATH] = TEXT("<unknown>");
    HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);

    if (NULL != h_process) {
        HMODULE h_mod;
        DWORD cb_needed;

        if (EnumProcessModules(h_process, &h_mod, sizeof(h_mod), &cb_needed)) {
            GetModuleBaseName(h_process, h_mod, s_processName, sizeof(s_processName) / sizeof(WCHAR));
        }
    }

    CloseHandle(h_process);

    std::wstring ws_processName(s_processName);

    return std::find(process_names.begin(), process_names.end(), ws_processName) != process_names.end()
}

void Manager::set_movement_lock(int locked) {
    emit movement_lock_change(bool(locked));
}

void Manager::set_never_show_tracker(bool state) {
    Data::update_settings(DISPLAY_TRACKER, json::value(state));
    emit instance().tracker_visibility(state);
}

void Manager::set_never_show_apm(bool state) {
    Data::update_settings(DISPLAY_APM, json::value(state));
    emit instance().apm_visibility(state);
}

void Manager::reset_session() {
    emit reset_session_data();
}

void Manager::refresh_keybinds() {
    INI::load_ini();
}

void Manager::set_settings_visible() {
    emit instance().show_settings();
}