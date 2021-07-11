#pragma once
#define NOMINMAX

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <Windows.h>
#include "utils/json.h"
#include "io/data.h"
#include "io/ini.h"
#include "utils/utils.h"

const std::wstring application = L"Path of Exile";
const std::vector<std::wstring> process_names = { L"PathOfExileSteam.exe", L"PathOfExile.exe", L"PathOfExile_x64.exe", L"PathOfExile_x64Steam.exe" };