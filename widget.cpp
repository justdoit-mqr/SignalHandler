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

    connect(signalHandler,SIGNAL(udiskStateSignal(int)),this,SLOT(udiskStateSlot(int)));

}

Widget::~Widget()
{
}

void Widget::udiskStateSlot(int udiskState)
{
    if(udiskState == SignalHandler::UDISK_CONNECT)
    {
        btn->setText("connect");
    }
    else if(udiskState == SignalHandler::UDISK_DISCONNECT)
    {
        btn->setText("disconnect");
    }
    else if(udiskState == SignalHandler::UDISK_UNKNOW)
    {
        btn->setText("unknow");
    }
}
