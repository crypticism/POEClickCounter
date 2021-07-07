#pragma once

namespace INI {
	void load_ini();
	bool is_skill_code(int value, bool toggled = false);
	bool is_flask_code(int);
	bool is_detonate_code(int);

	std::wstring find_skill_id(int, bool);
	bool is_keyboard_toggle_code(int);
}