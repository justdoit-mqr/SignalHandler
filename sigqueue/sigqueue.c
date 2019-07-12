/*
 * 使用sigqueue()发送指定信号给指定进程，信号携带额外参数
 *
 * Author:mqr 2019.6.26
 *	
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
 
int main (int argc , char * argv[]) 
{
 
    if ( 4 != argc )
	{
        printf ("Usage: ./sigqueue pid sig_num sig_value\n") ;
        exit (1);
    }
 
    pid_t pid = atoi(argv[1]);
	int sig = atoi(argv[2]);
	int sig_value = atoi(argv[3]);

    sigval_t sigval;
    sigval.sival_int = sig_value;
    int errcode = 0 ;

	if((errcode = sigqueue(pid,sig,sigval))<0)
	{
		printf("sigqueue error!\n");
		exit (1);
	}
    printf("sigqueue success!\n");
    return 0 ;
}

