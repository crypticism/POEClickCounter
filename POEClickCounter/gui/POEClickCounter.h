#pragma once

#include <QtWidgets/QWidget>
#include "ui_POEClickCounter.h"

class POEClickCounter : public QWidget
{
    Q_OBJECT

public:
    POEClickCounter(QWidget *parent = Q_NULLPTR);

private:
    Ui::POEClickCounterClass ui;
};
