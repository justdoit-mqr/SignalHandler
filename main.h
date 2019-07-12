#ifndef MAIN_H
#define MAIN_H

/* 枚举类型声明
 * 枚举常量保存在静态存储区,作用域为包涵该头文件的模块，同一作用域枚举常量不能重复定义。
 * 也可以将枚举类型声明放在某个类内，这样枚举常量的作用域就被限定在类的作用域内了，但需要注意
 * 如果该枚举类型作为信号槽的参数传递，那么就不能在类内声明，否则信号槽无法关联。
 */
enum UDiskState//U盘连接状态
{
    UDISK_DISCONNECT =0,
    UDISK_CONNECT=1,
};

#endif // MAIN_H
