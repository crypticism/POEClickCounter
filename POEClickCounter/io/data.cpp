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
		session.SetNamedValue(LEFT_CLICK, json::value(0.0));
		session.SetNamedValue(MIDDLE_CLICK, json::value(0.0));
		session.SetNamedValue(RIGHT_CLICK, json::value(0.0));
		session.SetNamedValue(SKILL_USE, json::value(0.0));
		session.SetNamedValue(FLASK_USE, json::value(0.0));
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
			update_settings(GUI_X_COORDINATE, json::value(500));
			update_settings(GUI_Y_COORDINATE, json::value(500));
			update_settings(DISPLAY_INDEX, json::value(0));
			update_settings(NEVER_SHOW_GUI, json::value(false));
			update_settings(COUNT_LEFT_CLICK_AS_SKILL_USE, json::value(false));
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