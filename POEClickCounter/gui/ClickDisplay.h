#pragma once
#ifndef CLICKDISPLAY_H
#define CLICKDISPLAY_H
#define NOMINMAX
#include "../pch.h"

#include <algorithm>

#include <QWidget>
#include <QtGui>

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
	~ClickDisplay();

    static LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboard_hook(int nCode, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

public slots:
    void dispatchUpdate(std::wstring);
    
    // Check whether the application is open or not
    void isApplicationActive() {
        // Reset checking value to false, as we can only determine whether it is open
        // not whether it is not open
        is_active_checking = false;

        EnumWindows(*EnumWindowsProc, NULL);

        if (is_active_checking) {
            this->show();
        }
        else {
            this->hide();
        }

        is_active = is_active_checking;
    }
    
    void setIsCheckingActive() {
        is_active_checking = true;
    }

protected:
    void mousePressEvent(QMouseEvent* evt) {
        old_pos = evt->globalPos();
    }

    void mouseMoveEvent(QMouseEvent* evt)
    {
        if (locked) {
            return;
        }
        
        const QPoint delta = evt->globalPos() - old_pos;
        move(x() + delta.x(), y() + delta.y());
        
        old_pos = evt->globalPos();
        File::update_settings(L"x_pos", json::value(old_pos.x()));
        File::update_settings(L"y_pos", json::value(old_pos.y()));
    }

signals:
    void handleEvent(std::wstring);
    void checkIsActive();
    void setIsActive();

private:
	Ui::ClickDisplay *ui;
	
    bool is_active_checking = false;
    bool is_active = false;

    bool locked = true;
    QPoint old_pos;

    HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;

    QIcon lock_icon = QIcon("gui\\lock.ico");
    QIcon unlock_icon = QIcon("gui\\unlock.ico");
};


#endif // CLICKDISPLAY_H