#include "widget.h"
#include <QVBoxLayout>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    btn = new QPushButton();
    btn->setText("status");

    QVBoxLayout *vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(btn);
    this->resize(200,200);

    //IPC信号处理
    SignalHandler *signalHandler = SignalHandler::getInstance();
    signalHandler->registerSignal(SignalHandler::UDISK_SIG);//注册需要监听处理的信号

    connect(signalHandler,SIGNAL(udiskSignal(UDiskState)),this,SLOT(udiskSlot(UDiskState)));

}

Widget::~Widget()
{
}

void Widget::udiskSlot(UDiskState state)
{
    if(state == UDISK_CONNECT)
    {
        btn->setText("connect");
    }
    else
    {
        btn->setText("disconnect");
    }
}
