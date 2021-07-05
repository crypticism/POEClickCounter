#pragma once

#ifndef DATA_H
#define DATA_H

#include "../pch.h"

extern const inline std::wstring LEFT_CLICK = L"left_click";
extern const inline std::wstring RIGHT_CLICK = L"right_click";
extern const inline std::wstring MIDDLE_CLICK = L"middle_click";
extern const inline std::wstring SKILL_USE = L"skill";
extern const inline std::wstring FLASK_USE = L"flask";

extern const inline std::wstring VISIBLE_COUNTS = L"visible_counts";

extern const inline std::wstring NEVER_SHOW_GUI = L"never_show";
extern const inline std::wstring DISPLAY_INDEX = L"display_index";
extern const inline std::wstring GUI_X_COORDINATE = L"x_pos";
extern const inline std::wstring GUI_Y_COORDINATE = L"y_pos";
extern const inline std::wstring COUNT_LEFT_CLICK_AS_SKILL_USE = L"count_left_click_as_skill";

namespace Data {
	const std::vector<std::wstring> fields = { LEFT_CLICK, MIDDLE_CLICK, RIGHT_CLICK, SKILL_USE, FLASK_USE };

	void reset_session_data();
	
	void save_data();
	void save_settings();
	
	double get_data_value(std::wstring);
	double get_session_value(std::wstring);

	void load_data();
	void load_settings();

	void update_data(std::wstring, json::JsonValue);
	void update_settings(std::wstring, json::JsonValue);
	void update_session(std::wstring, json::JsonValue);

	void set_count_visibility(std::wstring, json::JsonValue);
	bool is_count_visible(std::wstring);

	json::JsonObject& get_data();
	json::JsonObject& get_session_data();
	json::JsonObject& get_settings();
}

#endif // DATA_H
