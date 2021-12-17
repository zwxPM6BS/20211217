#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>


#define N 128
#define PORT 8888
#define IP "0.0.0.0"
//数据包协议
struct mess
{
	char type;           //类型:A:确认包 E:错误包 \
								L:查看 C:修改  Q:退出 R:注册  D:登录
	char user[32];
	char id[32];
	char buf[N];
	unsigned short pos;   //包编号
};

#define ERR_MSG(msg) do{\
	printf("__%d__:",__LINE__);\
	perror(msg);\
}while(0)


#endif