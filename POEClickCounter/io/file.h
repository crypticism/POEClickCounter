#pragma once

#include "../pch.h"

namespace File {
	std::wstring get_save_folder_location();
	std::wstring get_data_file_location();
	std::wstring get_settings_file_location();
	void save_data(const json::JsonObject&);
	void save_settings(const json::JsonObject&);
	json::JsonObject load_data();
	json::JsonObject load_settings();
}