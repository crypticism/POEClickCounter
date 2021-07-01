#include "../pch.h"

#include <filesystem>

#include <ShlObj.h>

namespace File
{
	constexpr const wchar_t* settings_filename = L"settings.json";
	constexpr const wchar_t* data_filename = L"click_data.json";

	std::wstring get_save_folder_location() {
		PWSTR local_app_path;
		winrt::check_hresult(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &local_app_path));
		std::wstring result{ local_app_path };
		CoTaskMemFree(local_app_path);

		result += L"\\POECounter\\";
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

	void save_data(const json::JsonObject& data) {
		// Avoid overwriting file with empty data if something goes wrong
		if (!json::has(data, L"left_click", json::JsonValueType::Number)) {
			return;
		}
		std::wstring save_file_location = get_data_file_location();
		json::to_file(save_file_location, data);
	}

	void save_settings(const json::JsonObject& settings) {
		const std::wstring save_file_location = get_settings_file_location();
		json::to_file(save_file_location, settings);
	}

	json::JsonObject load_data()
	{
		const std::wstring save_file_location = get_data_file_location();
		auto saved_data = json::from_file(save_file_location);
		return saved_data.has_value() ? std::move(*saved_data) : json::JsonObject{};
	}

	json::JsonObject load_settings()
	{
		const std::wstring save_file_location = get_settings_file_location();
		auto saved_settings = json::from_file(save_file_location);
		return saved_settings.has_value() ? std::move(*saved_settings) : json::JsonObject{};
	}
}