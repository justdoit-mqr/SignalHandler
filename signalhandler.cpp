/*
 *@author: 缪庆瑞
 *@date:   2019.6.24
 *@brief:  该类主要用来响应处理Linux的signal信号(与Qt的信号槽没有关系)
 */
#include "signalhandler.h"
#include <stdio.h>
#include <QCoreApplication>

QTimer SignalHandler::delayTimer;
QQueue<int> SignalHandler::sigNumQueue;
QQueue<int> SignalHandler::sigValQueue;
/*
 *@brief:   构造函数(私有)
 *@author:  缪庆瑞
 *@date:    2019.6.24
 */
SignalHandler::SignalHandler()
{
    delayTimer.setSingleShot(true);//单次定时器
    //连接静态定时器超时信号到该单例对象的槽中
    connect(&delayTimer,SIGNAL(timeout()),this,SLOT(delayTimerSlot()));
}
/*
 *@brief:   信号注册
 *@author:  缪庆瑞
 *@date:    2019.6.24
 *@param:   sigNum:信号值(最好取SIGRTMIN+3之后的值，前几个好像被内部使用了)
 *@param:   isSigIGN:是否选择忽略信号
 */
void SignalHandler::registerSignal(int sigNum, bool isSigIGN)
{
    struct sigaction act;//用来注册信号
    if(isSigIGN)
    {
        act.sa_handler = SIG_IGN;
    }
    else
    {
        act.sa_sigaction = signalHandle;//指定回调函数，三个函参，能接受额外数据
        act.sa_flags = SA_SIGINFO;//与sa_sigaction搭配使用，接受额外数据
    }
    sigemptyset(&act.sa_mask);//清空屏蔽字
    //为sigNum指定回调函数
    sigaction(sigNum,&act,NULL);
}
/*
 *@brief:   信号处理函数(类似于中断处理函数)
 * 注1:由信号触发时该函数会中断当前程序(其他函数)的执行，等处理结束再回到中断之前的位置继续运行。
 * 所以处理不宜太繁琐，以保证尽快恢复到被打断之前的状态，避免中断前的执行逻辑受影响，像一些
 * 不紧要的操作(比如刷新ui显示等)开一个定时器延后交由Qt主事件循环处理
 * 注2:该处理函数内部不可以使用qDebug打印信息，实测加入qDebug经常会卡在中断里退不出去。应该是
 * qDebug内部处理机制与中断之前的处理(比如QProcess::start()启动进程操作)互斥，产生了死锁现象。
 *@author:  缪庆瑞
 *@date:    2019.6.24
 *@param:   sig:信号值
 *@param:   s_t:信号所携带的额外数据
 *@param:   p:posix没有规范使用该参数的标准,暂不使用
 */
void SignalHandler::signalHandle(int sig, siginfo_t *s_t, void *p)
{
    Q_UNUSED(p);//该参数暂时不用
    //将信号值和携带的整形参数放入队列
    sigNumQueue.enqueue(sig);
    sigValQueue.enqueue(s_t->si_int);
    //qDebug()<<"sigNum:"<<sig<<"\tsigValue:"<<s_t->si_int;
    printf("sigNum:%d\tsigValue:%d\n",sig,s_t->si_int);

    /* 由于该函数是由linux底层信号触发的，所以它实际运行时是脱离了Qt的主事件循环的，这就导致
     * 下面的延时定时器超时后并不会立即响应槽，直到整个程序重新进入到Qt的主事件循环(比如界面
     * 刷新等)。假如该函数被信号触发执行完毕后的一段时间内都没有重新进入Qt主事件循环的操作，
     * 那么延时定时器的槽将一直不会响应，界面可能无法实时刷新。为了解决该问题，经过查询资料
     * 以及不断的测试，最终找到一个方法就是使用QCoreApplication::postEvent()，该方法主动向某个
     * 对象发送一个事件，进而重新进入Qt的主事件循环。*/
    delayTimer.start(10);
    //其实这里主要是为了重新进入Qt的主事件循环，向任何对象发送任何事件都可以
    QCoreApplication::postEvent(&delayTimer,new QEvent(QEvent::None));
}
/*
 *@brief:   由信号触发的不紧要的处理放在这里，内部按照队列先入先出的顺序处理当前队列中的信号。
 * 因为该函数是通过Qt的主事件循环进行异步处理调用的，所以当主事件循环阻塞或者信号短时间频繁
 * 发生状态改变时可能会出现状态同步延迟的现象。
 *@author:  缪庆瑞
 *@date:    2019.6.25
 */
void SignalHandler::delayTimerSlot()
{
    //确保信号值和携带的参数一一对应
    if(sigNumQueue.size() != sigValQueue.size())
    {
        return;
    }
    //处理队列中的信号
    while(!sigNumQueue.isEmpty())
    {
        int sigNum = sigNumQueue.dequeue();
        int sigVal = sigValQueue.dequeue();
        switch (sigNum) {
        case UDISK_SIG:
            if(sigVal==UDISK_CONNECT)
            {
                emit udiskStateSignal(UDISK_CONNECT);
            }
            else if(sigVal==UDISK_DISCONNECT)
            {
                emit udiskStateSignal(UDISK_DISCONNECT);
            }
            else
            {
                emit udiskStateSignal(UDISK_UNKNOW);
            }
            break;
        default:
            break;
        }
    }
}
