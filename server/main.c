#include"factory.h"
int exit_fds[2];
void sig_exit_func(int sig_num)
{
	char flag=1;
	write(exit_fds[1],&flag,1);
}
void *download_file(void *p)

{
	pfac pf=(pfac)p;
	pque_t pq=&pf->que;
	pnode_t pcur;
	while(1)//加锁后判断队列是否为空，为空阻塞，否取队列头，解锁，传输文件，释放空间
	{
		pthread_mutex_lock(&pq->que_mutex);
		if(0==pq->size)
		{
			pthread_cond_wait(&pf->cond,&pq->que_mutex);
		}
		que_get(pq,&pcur);
		pthread_mutex_unlock(&pq->que_mutex);
		if(pcur!=NULL)
		{
			tran_file(pcur->new_fd);
			free(pcur);//一定要释放节点内存
		}
	}
}
int main(int argc,char **argv)
{
	args_check(5,argc);
	factory  f;//线程池结构体
	int thread_num=atoi(argv[3]);
	int capacity=atoi(argv[4]);
	factory_init(&f,download_file,thread_num,capacity);//初始化线程池
	factory_start(&f);
	int sfd;
	tcp_start_listen(&sfd,argv[1],argv[2],capacity);
	int new_fd;
	pque_t pq=&f.que;
	pnode_t pnew;
	pipe(exit_fds);//epoll监控
	int epfd=epoll_create(1);
	struct epoll_event event,evs[2];
	event.events=EPOLLIN;
	event.data.fd=sfd;
	int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	check_error(-1,ret,"epoll_ctl");
	event.data.fd=exit_fds[0];
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,exit_fds[0],&event);
	check_error(-1,ret,"epoll_ctl");
	int i;
	signal(SIGUSR1,sig_exit_func);
	while(1)
	{
		ret=epoll_wait(epfd,evs,2,-1);
		for(i=0;i<ret;i++)
		{
			if(sfd==evs[i].data.fd)
			{
				new_fd=accept(sfd,NULL,NULL);//接收到客户端的请求
				pnew=(pnode_t)calloc(1,sizeof(node_t));
				pnew->new_fd=new_fd;//把描述符赋值给节点的new_fd
				pthread_mutex_lock(&pq->que_mutex);
				que_insert(pq,pnew);//把作业放进队列中
				pthread_mutex_unlock(&pq->que_mutex);//解锁
				pthread_cond_signal(&f.cond);//唤醒线程
			}
			if(exit_fds[0]==evs[i].data.fd)
			{
				close(sfd);
				pnew=(pnode_t)calloc(1,sizeof(node_t));
				pnew->new_fd=-1;
				pthread_mutex_lock(&pq->que_mutex);
				que_insert(pq,pnew);
				pthread_mutex_unlock(&pq->que_mutex);
				pthread_cond_broadcast(&f.cond);
				que_insert_exit(pq,pnew);//在队列头插入标志节点
				for(i=0;i<f.pthread_num;i++)
				{
					pthread_join(f.pth_id[i],NULL);//等待子线程退出
				}
				exit(0);
			}
		}
	}
}
