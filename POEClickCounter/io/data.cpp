#include "../pch.h"

#include <filesystem>
#include <ShlObj.h>

#include "data.h"

namespace Data
{
	constexpr const wchar_t* settings_filename = L"settings.json";
	constexpr const wchar_t* data_filename = L"click_data.json";

	json::JsonObject data;
	json::JsonObject session;
	json::JsonObject settings;

	std::wstring get_save_folder_location() {
		PWSTR local_app_path;
		winrt::check_hresult(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &local_app_path));
		std::wstring result{ local_app_path };
		CoTaskMemFree(local_app_path);

		result += L"\\POEClickCounter\\";
		std::filesystem::path save_path(result);
		if (!std::filesystem::exists(save_path))
		{
			std::filesystem::create_directories(save_path);
		}
		return result;
	}

	std::wstring get_data_file_location() {
		return get_save_folder_location() + data_filename;
	}

	std::wstring get_settings_file_location() {
		return get_save_folder_location() + settings_filename;
	}

	void reset_session_data() {
		session.SetNamedValue(LEFT_CLICK, json::value(0));
		session.SetNamedValue(MIDDLE_CLICK, json::value(0));
		session.SetNamedValue(RIGHT_CLICK, json::value(0));
		session.SetNamedValue(SKILL_USE, json::value(0));
		session.SetNamedValue(FLASK_USE, json::value(0));
		session.SetNamedValue(DETONATE, json::value(0));

		json::JsonObject skill_specific_counts;
		skill_specific_counts.SetNamedValue(SKILL_1, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_2, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_3, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_4, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_5, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_6, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_7, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_8, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_9, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_10, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_11, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_12, json::value(0));
		skill_specific_counts.SetNamedValue(SKILL_13, json::value(0));

		json::JsonObject flask_specific_counts;
		flask_specific_counts.SetNamedValue(FLASK_1, json::value(0));
		flask_specific_counts.SetNamedValue(FLASK_2, json::value(0));
		flask_specific_counts.SetNamedValue(FLASK_3, json::value(0));
		flask_specific_counts.SetNamedValue(FLASK_4, json::value(0));
		flask_specific_counts.SetNamedValue(FLASK_5, json::value(0));

		session.SetNamedValue(SKILL_SPECIFIC_COUNTS, json::value(skill_specific_counts));
		session.SetNamedValue(FLASK_SPECIFIC_COUNTS, json::value(flask_specific_counts));
	}

	// Load data if present otherwise initialize all values to 0
	void load_data()
	{
		const std::wstring save_file_location = get_data_file_location();
		auto saved_data = json::from_file(save_file_location);
		if (saved_data.has_value()) {
			data = std::move(*saved_data);
		}
		else {
			data = json::JsonObject{};
			update_data(LEFT_CLICK, json::value(0));
			update_data(MIDDLE_CLICK, json::value(0));
			update_data(RIGHT_CLICK, json::value(0));
			update_data(SKILL_USE, json::value(0));
			update_data(FLASK_USE, json::value(0));
			update_data(DETONATE, json::value(0));

			json::JsonObject skill_specific_counts;
			skill_specific_counts.SetNamedValue(SKILL_1, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_2, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_3, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_4, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_5, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_6, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_7, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_8, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_9, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_10, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_11, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_12, json::value(0));
			skill_specific_counts.SetNamedValue(SKILL_13, json::value(0));

			json::JsonObject flask_specific_counts;
			flask_specific_counts.SetNamedValue(FLASK_1, json::value(0));
			flask_specific_counts.SetNamedValue(FLASK_2, json::value(0));
			flask_specific_counts.SetNamedValue(FLASK_3, json::value(0));
			flask_specific_counts.SetNamedValue(FLASK_4, json::value(0));
			flask_specific_counts.SetNamedValue(FLASK_5, json::value(0));

			update_data(SKILL_SPECIFIC_COUNTS, json::value(skill_specific_counts));
			update_data(FLASK_SPECIFIC_COUNTS, json::value(flask_specific_counts));
			save_data();
		}
	}

	// Loads settings if present otherwise initalizes a settings object
	void load_settings()
	{
		const std::wstring save_file_location = get_settings_file_location();
		auto saved_settings = json::from_file(save_file_location);
		if (saved_settings.has_value()) {
			settings = std::move(*saved_settings);
		}
		else {
			settings = json::JsonObject{};
			update_settings(TRACKER_X_COORDINATE, json::value(500));
			update_settings(TRACKER_Y_COORDINATE, json::value(500));

			update_settings(APM_X_COORDINATE, json::value(400));
			update_settings(APM_Y_COORDINATE, json::value(400));

			update_settings(DISPLAY_INDEX, json::value(0));
			update_settings(DISPLAY_TRACKER, json::value(true));
			update_settings(DISPLAY_APM, json::value(true));
			update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(false));
			update_settings(TRACK_DETONATE, json::value(false));

			// Initialize visibility values
			json::JsonObject visible_counts;
			visible_counts.SetNamedValue(LEFT_CLICK, json::value(true));
			visible_counts.SetNamedValue(MIDDLE_CLICK, json::value(true));
			visible_counts.SetNamedValue(RIGHT_CLICK, json::value(true));
			visible_counts.SetNamedValue(SKILL_USE, json::value(true));
			visible_counts.SetNamedValue(FLASK_USE, json::value(true));
			visible_counts.SetNamedValue(DETONATE, json::value(false));

			update_settings(VISIBLE_COUNTS, json::value(visible_counts));
			save_settings();
		}

	}

	void save_data() {
		// Avoid overwriting file with empty data if something goes wrong
		if (json::has(data, LEFT_CLICK, json::JsonValueType::Number) == false) {
			return;
		}
		std::wstring save_file_location = get_data_file_location();
		json::to_file(save_file_location, data);
	}

	void save_settings() {
		const std::wstring save_file_location = get_settings_file_location();
		json::to_file(save_file_location, settings);
	}

	double get_data_value(std::wstring field) {
		return data.GetNamedNumber(field);
	}

	double get_data_specific_skill_value(std::wstring skill_id) {
		return data.GetNamedObject(SKILL_SPECIFIC_COUNTS).GetNamedNumber(skill_id);
	}

	double get_data_specific_flask_value(std::wstring flask_id) {
		return data.GetNamedObject(FLASK_SPECIFIC_COUNTS).GetNamedNumber(flask_id);
	}

	double get_session_value(std::wstring field) {
		return session.GetNamedNumber(field);
	}

	void update_data(std::wstring field, json::JsonValue value) {
		data.SetNamedValue(field, value);
	}

	void update_settings(std::wstring field, json::JsonValue value) {
		settings.SetNamedValue(field, value);
	}

	void update_session(std::wstring field, json::JsonValue value) {
		session.SetNamedValue(field, value);
	}

	void update_data_specific_skill_value(std::wstring skill_id, json::JsonValue value) {
		data.GetNamedObject(SKILL_SPECIFIC_COUNTS).SetNamedValue(skill_id, value);
	}

	void update_data_specific_flask_value(std::wstring flask_id, json::JsonValue value) {
		data.GetNamedObject(FLASK_SPECIFIC_COUNTS).SetNamedValue(flask_id, value);
	}

	void set_count_visibility(std::wstring field, json::JsonValue value) {
		settings.GetNamedObject(VISIBLE_COUNTS).SetNamedValue(field, value);
	}

	bool is_count_visible(std::wstring field) {
		return settings.GetNamedObject(VISIBLE_COUNTS).GetNamedBoolean(field);
	}

	json::JsonObject& get_data() {
		return data;
	}

	json::JsonObject& get_session_data() {
		return session;
	}

	json::JsonObject& get_settings() {
		return settings;
	}	
}