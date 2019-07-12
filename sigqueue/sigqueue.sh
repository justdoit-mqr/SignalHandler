#!/bin/bash

sigqueueFunc() {
	#通过进程名获取进程号
	qt_pid=`pidof SignalHandler`
	if [ -z $qt_pid ]
	then 
		echo "sigqueue:there is no SignalHandler process." #1>/dev/console
		return -1
	fi
	#调用sigqueue 发送信号
	./sigqueue $qt_pid 38 $1 #1>/dev/console
}

sigqueueFunc $1
