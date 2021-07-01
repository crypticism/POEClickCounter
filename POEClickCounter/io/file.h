#pragma once

#include "../pch.h"

namespace File {
	void save_data();
	void save_settings();

	void load_data();
	void load_settings();

	void update_settings(std::wstring, json::JsonValue);

	json::JsonObject& get_data();
	json::JsonObject& get_settings();
}