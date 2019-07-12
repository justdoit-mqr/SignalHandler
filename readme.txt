SignalHandler是一个基于Qt编写的便捷类，主要用来响应处理Linux IPC的signal信号

SignalHandler类被设计为单例模式，使用时一般只需要调用registerSignal(int sigNum, bool isSigIGN=false)函数，注册需要监听处理的信号，然后在delayTimerSlot()函数中添加对指定信号的处理(通常采用qt的信号槽机制，emit一个信号，具体怎么处理由关联的槽去实现，尽量保持低耦合)即可。使用示例：
//IPC信号处理
SignalHandler *signalHandler = SignalHandler::getInstance();
signalHandler->registerSignal(SignalHandler::UDISK_SIG);//注册需要监听处理的信号
connect(signalHandler,SIGNAL(udiskSignal(UDiskState)),this,SLOT(udiskSlot(UDiskState)));


为了方便理解SignalHandler类的使用方式及其作用，该例程利用信号值38模拟了一个U盘插拔信号，程序启动时会注册监听该信号。然后可以在命令行中调用脚本(sigqueue目录中有提供)向进程发送38信号并携带一个参数，此时SignalHandler进程界面会根据信号参数值作出不同响应。
sigqueue:该目录下的sigqueue.c文件经过make编译后会生成一个sigqueue可执行命令(用来给指定进程发送指定信号，并可以携带一个额外参数)，然后执行sigqueue.sh脚本（带一个参数0或者1），就可以发送模拟的U盘插拔信号了。


作者联系方式：@为-何-而来(新浪微博)

