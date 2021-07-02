#include "../pch.h"

#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include "ini.h"

namespace INI {
	constexpr const wchar_t* poe_path = L"\\Documents\\My Games\\Path of Exile\\";
	constexpr const wchar_t* config_filename = L"production_Config.ini";

	std::vector< int > skill_codes;
	std::vector< int > flask_codes;

	std::wstring get_ini_location() {
		PWSTR ini_path;
		winrt::check_hresult(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &ini_path));
		std::wstring result{ ini_path };
		CoTaskMemFree(ini_path);

		return result + poe_path + config_filename;
	}

	void load_ini() {
		skill_codes.clear();
		flask_codes.clear();

		std::wstring save_file_location = get_ini_location();
		std::ifstream infile(save_file_location);

		std::string line;
		while (infile >> line) {
			if (line._Starts_with("use_bound_skill")) {
				auto command = line.substr(line.find("=") + 1);
				if (command.find(" ")) {
					command = command.substr(0, command.find(" "));
				}
				skill_codes.push_back(stoi(command));
				continue;
			}

			if (line._Starts_with("use_flask_in_slot")) {
				auto command = line.substr(line.find("=") + 1);
				flask_codes.push_back(stoi(command));
				continue;
			}
		}
	}

	bool is_skill_code(int value) {
		return std::find(skill_codes.begin(), skill_codes.end(), value) != skill_codes.end();
	}

	bool is_flask_code(int value) {
		return std::find(flask_codes.begin(), flask_codes.end(), value) != flask_codes.end();
	}
}