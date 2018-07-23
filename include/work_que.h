#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include"head.h"
//管理子线程队列而定义的结构体
typedef struct tag_node{
	int new_fd;
	struct tag_node *pNext;
}node_t,*pnode_t;
//子进程的结构体链表
typedef struct{
	pnode_t que_head,que_tail;//每一个结构体
	int que_capacity;
	int size;
	pthread_mutex_t que_mutex;
}que_t,*pque_t;
void que_insert(pque_t,pnode_t);
void que_get(pque_t,pnode_t*);
void que_insert_exit(pque_t,pnode_t);
#endif
