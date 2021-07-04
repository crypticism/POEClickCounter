#include "../pch.h"

#include <filesystem>
#include <ShlObj.h>

#include "file.h"

namespace File
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

	void save_data() {
		// Avoid overwriting file with empty data if something goes wrong
		if (!json::has(data, LEFT_CLICK, json::JsonValueType::Number)) {
			return;
		}
		std::wstring save_file_location = get_data_file_location();
		json::to_file(save_file_location, data);
	}

	void update_settings(std::wstring field, json::JsonValue value) {
		settings.SetNamedValue(field, value);
	}

	void save_settings() {
		const std::wstring save_file_location = get_settings_file_location();
		json::to_file(save_file_location, settings);
	}

	void load_data()
	{
		const std::wstring save_file_location = get_data_file_location();
		auto saved_data = json::from_file(save_file_location);
		data = saved_data.has_value() ? std::move(*saved_data) : json::JsonObject{};
	}

	void load_settings()
	{
		const std::wstring save_file_location = get_settings_file_location();
		auto saved_settings = json::from_file(save_file_location);
		settings = saved_settings.has_value() ? std::move(*saved_settings) : json::JsonObject{};
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