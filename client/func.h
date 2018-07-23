#include<stdio.h>
#include<sys/uio.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<pthread.h>
#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<wait.h>
#include<signal.h>
#include<sys/msg.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<string.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<dirent.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<sys/select.h>
#define args_check(a,b) {if(a!=b) {printf("error args\n");return -1;}}
#define check_error(ret_val,ret,func_name) {if(ret_val==ret)\
	{perror(func_name);return -1;}}
typedef struct
{
	pid_t pid;
	int fds;//子进程管道对端
	short busy_flag;//子进程状态标志，闲为0，忙为1
}Data_t,*pData_t;
typedef struct
{
	int len;
	char buf[1000];
}train;
int creat_child(pData_t,int);//声明创建子进程函数
int send_fd(int,int);//父进程传递文件描述符
int recv_fd(int,int*);//子进程接收文件描述符
void child_handle(int);//子进程调用函数（执行任务）
int tran_file(int);//传送文件函数
int send_n(int,char*,int);//解决速度不平衡的循环发送函数
int recv_n(int,char*,int);
#define FILENAME "file"
