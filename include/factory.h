#ifndef __FACTORY_H__
#define __FACTORY_H__
#include"head.h"
#include"work_que.h"
typedef void *(*thread_func_t)(void*p);
//管理线城信息的结构体
typedef struct{
	pthread_t *pth_id;//线程id
	que_t que;
	int pthread_num;//线程数量
	thread_func_t threadfunc;//线程的工作，即线程调用函数
	pthread_cond_t cond;//线程条件变量
	short start_flag;//线程状态标志
}factory,*pfac;
int factory_init(pfac,thread_func_t,int,int);//线程创建函数
int factory_start(pfac);//
int tcp_start_listen(int*,char*,char*,int);//tcp开始监听
int tran_file(int);//传送文件
int send_n(int,char*,int);//发送文件
#define FILENAME "file"
typedef struct{
	int len;
	char buf[1000];
}train;
#endif
