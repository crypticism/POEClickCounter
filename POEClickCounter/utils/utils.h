#pragma once

#include "../pch.h"

#include <QString>

const QString suffixes[6] = { "", "K", "M", "B", "T", "Q" };

// Converts a double value into a truncated string with a suffix to save space
QString calculateLabel(double value) {
    double c = value == 0.0 ? 0.0 : floor(log10(abs(value)) / 3);
    double m = std::min(5.0, c);
    double max = std::max(0.0, m);

    int index = max;

    std::string buffer = std::to_string(int(floor(value)));

    for (int i = 0; i < 3 * (index - 1); i++) {
        buffer.pop_back();
    }

    if (index >= 1) {
        std::string to_insert = ".";
        int pos = int(buffer.length()) - 3;
        buffer.insert(pos, to_insert);
    }

    return QString::fromUtf8(buffer).append(suffixes[index]);
};

// Find the name of the window that cursor is currently over
// Seems to correctly grab only the top-most window
std::wstring get_active_window() {
    POINT point;
    GetCursorPos(&point);
    HWND new_hwnd = WindowFromPoint(point);

    int nTitleLength = GetWindowTextLength(new_hwnd) + 1;

    std::wstring s(nTitleLength, '\0');
    LPWSTR title = reinterpret_cast<LPWSTR>(&s[0]);

    GetWindowText(new_hwnd, title, nTitleLength);
    return title;
}