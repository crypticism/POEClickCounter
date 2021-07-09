#pragma once

#ifndef DATA_H
#define DATA_H

#include "../pch.h"

extern const inline std::wstring LEFT_CLICK = L"left_click";
extern const inline std::wstring RIGHT_CLICK = L"right_click";
extern const inline std::wstring MIDDLE_CLICK = L"middle_click";
extern const inline std::wstring SKILL_USE = L"skill";
extern const inline std::wstring FLASK_USE = L"flask";
extern const inline std::wstring DETONATE = L"detonate";

extern const inline std::wstring VISIBLE_COUNTS = L"visible_counts";

extern const inline std::wstring NEVER_SHOW_GUI = L"never_show";
extern const inline std::wstring DISPLAY_INDEX = L"display_index";
extern const inline std::wstring COUNT_LEFT_CLICK_AS_SKILL_USE = L"count_left_click_as_skill";
extern const inline std::wstring TRACK_DETONATE = L"track_detonate";

extern const inline std::wstring TRACKER_X_COORDINATE = L"tracker_x_pos";
extern const inline std::wstring TRACKER_Y_COORDINATE = L"tracker_y_pos";
extern const inline std::wstring APM_X_COORDINATE = L"apm_x_pos";
extern const inline std::wstring APM_Y_COORDINATE = L"apm_y_pos";

extern const inline std::wstring SKILL_SPECIFIC_COUNTS = L"skill_specific_counts";
extern const inline std::wstring FLASK_SPECIFIC_COUNTS = L"flask_specific_counts";

extern const inline std::wstring FLASK_1 = L"flask_1";
extern const inline std::wstring FLASK_2 = L"flask_2";
extern const inline std::wstring FLASK_3 = L"flask_3";
extern const inline std::wstring FLASK_4 = L"flask_4";
extern const inline std::wstring FLASK_5 = L"flask_5";

extern const inline std::wstring SKILL_1 = L"skill_1";
extern const inline std::wstring SKILL_2 = L"skill_2";
extern const inline std::wstring SKILL_3 = L"skill_3";
extern const inline std::wstring SKILL_4 = L"skill_4";
extern const inline std::wstring SKILL_5 = L"skill_5";
extern const inline std::wstring SKILL_6 = L"skill_6";
extern const inline std::wstring SKILL_7 = L"skill_7";
extern const inline std::wstring SKILL_8 = L"skill_8";
extern const inline std::wstring SKILL_9 = L"skill_9";
extern const inline std::wstring SKILL_10 = L"skill_10";
extern const inline std::wstring SKILL_11 = L"skill_11";
extern const inline std::wstring SKILL_12 = L"skill_12";
extern const inline std::wstring SKILL_13 = L"skill_13";

namespace Data {
	const std::vector<std::wstring> fields = { LEFT_CLICK, MIDDLE_CLICK, RIGHT_CLICK, SKILL_USE, FLASK_USE };

	void reset_session_data();
	
	void save_data();
	void save_settings();
	
	double get_data_value(std::wstring);
	double get_session_value(std::wstring);
	double get_data_specific_flask_value(std::wstring);
	double get_data_specific_skill_value(std::wstring);

	void load_data();
	void load_settings();

	void update_data(std::wstring, json::JsonValue);
	void update_settings(std::wstring, json::JsonValue);
	void update_session(std::wstring, json::JsonValue);
	void update_data_specific_flask_value(std::wstring, json::JsonValue);
	void update_data_specific_skill_value(std::wstring, json::JsonValue);

	void set_count_visibility(std::wstring, json::JsonValue);
	bool is_count_visible(std::wstring);

	json::JsonObject& get_data();
	json::JsonObject& get_session_data();
	json::JsonObject& get_settings();
}

#endif // DATA_H
