/*
 *@author: 缪庆瑞
 *@date:   2019.6.24
 *@brief:  该类主要用来响应处理Linux的signal信号(与Qt的信号槽没有关系)，信号可以来源于内核态
 * 的驱动程序(一般使用固定的信号值SIGIO)或者由其他进程(IPC，可以自定义SIGRTMIN--SIGRTMAX
 * 信号的功能)发出。(目前该类主要针对IPC的信号进行处理)
 *
 * 信号安装使用sigaction()函数，支持信号带参数。信号的处理类似于异步IO，在信号处理函数中
 * 仅仅保存接收的信号值和额外参数，真正的处理通过定时器交给Qt的主事件循环。
 *
 * 因为类成员函数做信号处理函数必须设置成静态的，所以该类中需要定义一些静态成员变量供静态函数
 * 访问。为了避免日后该类的多个对象访问静态成员变量造成不被预期的影响，这里将该类设计为单例
 * 模式，并尽可能的简化信号处理函数的操作，降低对被信号中断前代码执行逻辑的影响。
 *
 */
#include "signalhandler.h"
#include <QDebug>
#include <QCoreApplication>

QTimer SignalHandler::delayTimer;
QMap<int,int> SignalHandler::sigNumAndValue;
int SignalHandler::currentSigNum;
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
    //为SIGRTMIN指定回调函数
    sigaction(sigNum,&act,NULL);
}
/*
 *@brief:   信号处理函数     类似于中断处理函数
 * 由信号触发时该函数会中断当前程序(其他函数)的执行，等处理结束再回到中断之前的位置继续运行。
 * 所以处理不宜太繁琐，以保证尽快恢复到被打断之前的状态，避免中断前的执行逻辑受影响，像一些
 * 不紧要的操作(比如刷新ui显示等)开一个定时器延后交由Qt主事件循环处理
 *@author:  缪庆瑞
 *@date:    2019.6.24
 *@param:   sig:信号值
 *@param:   s_t:信号所携带的额外数据
 *@param:   p:posix没有规范使用该参数的标准,暂不使用
 */
void SignalHandler::signalHandle(int sig, siginfo_t *s_t, void *p)
{
    Q_UNUSED(p);//该参数暂时不用
    sigNumAndValue.insert(sig,s_t->si_int);//保存信号值以及所携带的参数
    currentSigNum = sig;//保存当前到来的信号，以供延时函数知道由哪个信号触发
    qDebug()<<"sigNum:"<<sig<<"\tsigValue:"<<s_t->si_int;

    /* 由于该函数是由linux底层信号触发的，所以它实际运行时是脱离了Qt的主事件循环的，这就导致
     * 下面的延时定时器超时后并不会立即响应槽，直到整个程序重新进入到Qt的主事件循环(比如界面
     * 刷新等)。假如该函数被信号触发执行完毕后的一段时间内都没有重新进入Qt主事件循环的操作，
     * 那么延时定时器的槽将一直不会响应，界面可能无法实时刷新。为了解决该问题，经过查询资料
     * 以及不断的测试，最终找到一个方法就是使用QCoreApplication::postEvent()，该方法主动向某个
     * 对象发送一个事件，进而重新进入Qt的主事件循环。
     */
    delayTimer.start(10);
    //其实这里主要是为了重新进入Qt的主事件循环，向任何对象发送任何事件都可以
    QCoreApplication::postEvent(&delayTimer,new QEvent(QEvent::None));
}
/*
 *@brief:   由信号触发的不紧要的处理放在这里，该函数中根据currentSigNum变量的值只处理最新
 * 到来的信号，但有一个弊端就是在极端条件下(比如多个信号在很短的时间内连续发送)可能会漏掉
 * 一些信号。暂时先这样处理，遇到具体问题再解决。
 *@author:  缪庆瑞
 *@date:    2019.6.25
 */
void SignalHandler::delayTimerSlot()
{
    //qDebug()<<"delayTimerSlot.............";
    int sig_int;
    switch (currentSigNum) {
    case UDISK_SIG:
         sig_int = sigNumAndValue.value(UDISK_SIG,-1);//读取U盘信号携带的参数值
        if(sig_int==UDISK_CONNECT)
        {
            emit udiskSignal(UDISK_CONNECT);
        }
        else if(sig_int==UDISK_DISCONNECT)
        {
            emit udiskSignal(UDISK_DISCONNECT);
        }
        break;
    default:
        break;
    }
}
