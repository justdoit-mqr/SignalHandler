# SignalHandler
这是一个基于Qt编写的便捷类，主要用来响应处理Linux IPC的signal信号。  
## 功能概述
类内部的信号安装使用sigaction()函数，支持信号带参数。信号的处理类似于异步IO，在信号处理函数中仅仅保存接收的信号值和额外参数，真正的处理通过定时器交给Qt的主事件循环。  
因为类成员函数做信号处理函数必须设置成静态的，所以该类中需要定义一些静态成员变量供静态函数访问。为了避免日后该类的多个对象访问静态成员变量造成不被预期的影响，这里将该类设计为单例模式，并尽可能的简化信号处理函数的操作，降低对被信号中断前代码执行逻辑的影响。  
## 代码接口
SignalHandler类的对外接口目前仅提供了`void registerSignal(int sigNum, bool isSigIGN=false)`函数，用于注册需要监听处理的信号。然后在delayTimerSlot()函数中添加对指定信号的处理(通常采用qt的信号槽机制，emit一个信号，具体怎么处理由关联的槽去实现，尽量保持低耦合)即可。  
## 使用示例
```
//IPC信号处理
SignalHandler *signalHandler = SignalHandler::getInstance();
signalHandler->registerSignal(SignalHandler::UDISK_SIG);//注册需要监听处理的信号
connect(signalHandler,SIGNAL(udiskStateSignal(int)),this,SLOT(udiskStateSlot(int)));

```
为了方便理解SignalHandler类的使用方式及其作用，该例程利用信号值38模拟了一个U盘插拔信号，程序启动时会注册监听该信号。然后可以在命令行中调用脚本(sigqueue目录中有提供)向进程发送38信号并携带一个参数，此时SignalHandler进程界面会根据信号参数值作出不同响应。  
sigqueue:该目录下的sigqueue.c文件经过make编译后会生成一个sigqueue可执行命令(用来给指定进程发送指定信号，并可以携带一个额外参数)，然后执行sigqueue.sh脚本（带一个参数0或者1），就可以发送模拟的U盘插拔信号了。  


## 作者联系方式
**邮箱:justdoit_mqr@163.com**  
**新浪微博:@为-何-而来**  
