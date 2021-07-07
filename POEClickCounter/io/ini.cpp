#include "../pch.h"

#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <vector>

#include "ini.h"

int parse_int_from_id(std::string);

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

	// key = flask_id, val = key
	std::map<std::string, int> m_flaskIdToKey;

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
				// Find the final index of the skill string
				int substr_start = line.find("skill", 5) + 5;
				// Find how many characters the skill id is
				int num_char = line.find("=") - substr_start;
				// Get the skill id as "skill_#" by capturing the skill number and appending it to "skill_"
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
				int n_startOfSubstrIndex = line.find("slot", 4) + 4;
				int n_numberOfCharactersInFlaskSlot = line.find("=") - n_startOfSubstrIndex;
				std::string s_flaskId = "flask_" + line.substr(n_startOfSubstrIndex, n_numberOfCharactersInFlaskSlot);

				auto s_key = line.substr(line.find("=") + 1);

				if (s_key.find(" ")) {
					s_key = s_key.substr(0, s_key.find(" "));
				}
				int n_key = stoi(s_key);
				m_flaskIdToKey[s_flaskId] = n_key;
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
				int n_skillIdFromMap = parse_int_from_id(it.first);
				if ((toggled && n_skillIdFromMap >= n_StartOfSecondSkillbar) || (!toggled && n_skillIdFromMap < n_StartOfSecondSkillbar)) {
					output = true;
				}
			}
		}

		return output;
	}

	bool is_flask_code(int value) {
		bool output = false;

		for (auto& it : m_flaskIdToKey) {
			if (it.second == value) {
				output = true;
			}
		}

		return output;
	}

	bool is_detonate_code(int value) {
		return value == detonate;
	}

	bool is_keyboard_toggle_code(int value) {
		// CTRL is stored in the config, but isn't an actually produced code.
		// Thus if toggle_code is VK_CONTROL, check if the actual value is VK_LCONTROL or VK_RCONTROL
		if (toggle_code == VK_CONTROL && (value == VK_LCONTROL || value == VK_RCONTROL)) {
			return true;
		}

		// SHIFT is stored in the config, but isn't an actually produced code.
		// Thus if toggle_code is VK_SHIFT, check if the actual value is VK_LSHIFT or VK_RSHIFT
		if (toggle_code == VK_SHIFT && (value == VK_LSHIFT || value == VK_RSHIFT)) {
			return true;
		}

		// MENU/ALT is stored in the config, but isn't an actually produced code.
		// Thus if toggle_code is VK_MENU, check if the actual value is VK_LMENU or VK_RMENU
		if (toggle_code == VK_MENU && (value == VK_LMENU || value == VK_RMENU)) {
			return true;
		}

		return toggle_code == value;
	}

	std::wstring find_skill_id(int value, bool toggled) {
		// Having this identical block as in is_skill_code feels bad but reusing it seems worse
		for (auto& it : m_skillIdToKey) {
			if (it.second == value) {
				int n_skillIdFromMap = parse_int_from_id(it.first);
				if ((toggled && n_skillIdFromMap >= n_StartOfSecondSkillbar) || (!toggled && n_skillIdFromMap < n_StartOfSecondSkillbar)) {
					return std::wstring(it.first.begin(), it.first.end());
				}
			}
		}
		return L"";
	}

	std::wstring find_flask_id(int value) {
		for (auto& it : m_flaskIdToKey) {
			if (it.second == value) {
				return std::wstring(it.first.begin(), it.first.end());
			}
		}
		return L"";
	}
}

// Takes a string like "skill_#" or "flask_#" and returns the # as an int
int parse_int_from_id(std::string skill) {
	return stoi(skill.substr(skill.find("_") + 1));
}