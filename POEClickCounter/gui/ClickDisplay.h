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

public slots:
    void dispatchUpdate(std::wstring);
    void toggleLock() {
        locked = !locked;
        if (locked) {
            setIcon(lock_icon);
        }
        else {
            setIcon(unlock_icon);
        }
    };

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

    void setIcon(QIcon icon) {
        ui->lock_unlock_button->setIcon(icon);
    }

signals:
    void handleEvent(std::wstring);

private:
	Ui::ClickDisplay *ui;
	
    bool locked = true;
    QPoint old_pos;

    HHOOK hh_mouse_hook;
	HHOOK hh_keyboard_hook;

    QIcon lock_icon = QIcon("gui\\lock.ico");
    QIcon unlock_icon = QIcon("gui\\unlock.ico");
};

#endif // CLICKDISPLAY_H