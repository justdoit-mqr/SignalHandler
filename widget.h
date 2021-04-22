#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "signalhandler.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void udiskStateSlot(int udiskState);

private:
    QPushButton *btn;

};

#endif // WIDGET_H
