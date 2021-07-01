#pragma once
#ifndef CLICKDISPLAY_H
#define CLICKDISPLAY_H

#include "../pch.h"

#include <QWidget>

#include "../io/file.h"
#include "ui_ClickDisplay.h"

const QString suffixes[6] = { "", "K", "M", "B", "T", "Q" };

const std::wstring LEFT_CLICK = L"left_click";
const std::wstring MIDDLE_CLICK = L"middle_click";
const std::wstring RIGHT_CLICK = L"right_click";
const std::wstring SKILL_USE = L"skill";
const std::wstring FLASK_USE = L"flask";

class ClickDisplay : public QWidget
{
	Q_OBJECT
    Q_DISABLE_COPY(ClickDisplay)

public:
    static ClickDisplay& instance();
	explicit ClickDisplay(QWidget *parent = Q_NULLPTR);
	virtual ~ClickDisplay();

    static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam);

public slots:
    void dispatchUpdate(std::wstring event_type) {
        double nValue = getValue(event_type) + 1;
        this->data.SetNamedValue(event_type, json::value(nValue));

        QString sValue = calculateLabel(nValue);

        if (event_type == LEFT_CLICK) {
            ui->l_click_value->setText(sValue);
            return;
        }
        if (event_type == MIDDLE_CLICK) {
            ui->m_click_value->setText(sValue);
            return;
        }
        if (event_type == RIGHT_CLICK) {
            ui->r_click_value->setText(sValue);
            return;
        }
        if (event_type == SKILL_USE) {
            ui->skill_value->setText(sValue);
            return;
        }
        if (event_type == FLASK_USE) {
            ui->flask_value->setText(sValue);
            return;
        }
    };

signals:
    void handleEvent(std::wstring);

private:
	Ui::ClickDisplay *ui;
	
    HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;
    
    json::JsonObject data;

    double getValue(std::wstring event_type) {
        double value;
        if (json::has(this->data, event_type, json::JsonValueType::Number)) {
            return floor(this->data.GetNamedNumber(event_type));
        }
        else {
            value = 0.0;
        }
        return value;
    }
    QString calculateLabel(double value) {
        double cmp = value == 0 ? 0 : floor(log10(abs(value)) / 3);
        double min = std::min(5.0, cmp);
        double max = std::max(0.0, min);

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
};

#endif // CLICKDISPLAY_H