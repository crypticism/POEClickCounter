#pragma once

#ifndef FILE_H
#define FILE_H

#include "../pch.h"

extern const inline std::wstring LEFT_CLICK = L"left_click";
extern const inline std::wstring RIGHT_CLICK = L"right_click";
extern const inline std::wstring MIDDLE_CLICK = L"middle_click";
extern const inline std::wstring SKILL_USE = L"skill";
extern const inline std::wstring FLASK_USE = L"flask";

extern const inline std::wstring NEVER_SHOW_GUI = L"never_show";
extern const inline std::wstring DISPLAY_INDEX = L"display_index";
extern const inline std::wstring GUI_X_COORDINATE = L"x_pos";
extern const inline std::wstring GUI_Y_COORDINATE = L"y_pos";
extern const inline std::wstring COUNT_LEFT_CLICK_AS_SKILL_USE = L"count_left_click_as_skill";

namespace File {
	void reset_session_data();
	
	void save_data();
	void save_settings();

	void load_data();
	void load_settings();

	void update_settings(std::wstring, json::JsonValue);

	json::JsonObject& get_data();
	json::JsonObject& get_session_data();
	json::JsonObject& get_settings();
}

#endif // FILE_H
