#include "../pch.h"

#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <vector>

#include "ini.h"

int parse_int_from_skill_id(std::string);

namespace INI {
	constexpr const wchar_t* poe_path = L"\\Documents\\My Games\\Path of Exile\\";
	constexpr const wchar_t* config_filename = L"production_Config.ini";

	int n_StartOfSecondSkillbar = 9;

	std::vector< int > skill_codes;
	std::vector< int > flask_codes;
	int detonate;
	int toggle_code;

	// key = skill_id, val = key
	std::map<std::string, int> m_skillIdToKey;

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
				// Find the start of the skill id
				int substr_start = line.find("skill", 5) + 5;
				// Find how many characters the skill id is
				int num_char = line.find("=") - substr_start;
				// Get the skill id as "skill_#"
				std::string skill_id = "skill_" + line.substr(substr_start, num_char);

				// Get the actual keycode for the skill
				auto sKey = line.substr(line.find("=") + 1);

				// If there is a space, it is a toggle skill and the " 2" needs to be removed
				if (sKey.find(" ")) {
					sKey = sKey.substr(0, sKey.find(" "));
					
				}
				int nKey = stoi(sKey);
				m_skillIdToKey[skill_id] = nKey;
				continue;
			}

			if (line._Starts_with("use_flask_in_slot")) {
				auto command = line.substr(line.find("=") + 1);
				flask_codes.push_back(stoi(command));
				continue;
			}

			if (line._Starts_with("toggle_skill_bar")) {
				auto command = line.substr(line.find("=") + 1);
				toggle_code = stoi(command);
				continue;
			}

			if (line._Starts_with("detonate")) {
				auto command = line.substr(line.find("=") + 1);
				detonate = stoi(command);
				continue;
			}
		}
	}

	bool is_skill_code(int value, bool toggled) {
		bool output = false;

		for (auto& it : m_skillIdToKey) {
			if (it.second == value) {
				int n_skillIdFromMap = parse_int_from_skill_id(it.first);
				if ((toggled && n_skillIdFromMap >= n_StartOfSecondSkillbar) || (!toggled && n_skillIdFromMap < n_StartOfSecondSkillbar)) {
					output = true;
				}
			}
		}

		return output;
	}

	bool is_flask_code(int value) {
		return std::find(flask_codes.begin(), flask_codes.end(), value) != flask_codes.end();
	}

	bool is_detonate_code(int value) {
		return value == detonate;
	}

	bool is_keyboard_toggle_code(int value) {
		// Input value of 17 is CTRL, but does not specify which CTRL and will never match
		// an actual input. 162 and 163 are LCTRL and RCTRL respectively, so we must match
		// against those
		if (toggle_code == 17 && (value == 162 || value == 163)) {
			return true;
		}
		return toggle_code == value;
	}

	std::wstring find_skill_id(int value, bool toggled) {
		// Having this identical block as in is_skill_code feels bad but reusing it seems worse
		for (auto& it : m_skillIdToKey) {
			if (it.second == value) {
				int n_skillIdFromMap = parse_int_from_skill_id(it.first);
				if ((toggled && n_skillIdFromMap >= n_StartOfSecondSkillbar) || (!toggled && n_skillIdFromMap < n_StartOfSecondSkillbar)) {
					return std::wstring(it.first.begin(), it.first.end());
				}
			}
		}
		return L"";
	}
}

// Takes a string like "skill_#" or "flask_#" and returns the # as an int
int parse_int_from_skill_id(std::string skill) {
	return stoi(skill.substr(skill.find("_") + 1));
}