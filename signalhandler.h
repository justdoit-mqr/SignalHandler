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
#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include "signal.h"
#include "main.h"
#include <QObject>
#include <QTimer>
#include <QMap>

class SignalHandler : public QObject
{
    Q_OBJECT
public:
    //自定义Linux(SIGRTMIN--SIGRTMAX)信号值的功能，最好取SIGRTMIN+3之后的值，前几个好像被内部使用了
    enum SignalNum{
        UDISK_SIG=38,//用信号值38标记为U盘信号
    };

    //单例模式
    static SignalHandler * getInstance()
    {
        static SignalHandler signalHandler;
        return &signalHandler;
    }
    //信号安装注册
    void registerSignal(int sigNum, bool isSigIGN=false);
    //必须是静态成员函数，否则信号安装时会因为隐含的this指针造成参数不匹配，无法编译通过。
    static void signalHandle(int sig,siginfo_t *s_t,void *p);

private:
    SignalHandler();//私有构造函数
    static QTimer delayTimer;//延后处理定时器
    static QMap<int,int> sigNumAndValue;//保存信号值和携带的参数值
    static int currentSigNum;//当前要处理的信号值

signals:
    void udiskSignal(UDiskState state);

public slots:
    void delayTimerSlot();//延后处理
};

#endif // SIGNALHANDLER_H
