#ifndef __SERVER_H__
#define __SERVER_H__

#include "public.h"


int network_init(int *sfd);
int multithreaded_server(int sfd,sqlite3 *db);//多线程服务器
void *RcvCliMsg(void *arg);           //多线程服务器回调函数
int do_regist(struct pth_info pinfo);
int do_login(struct pth_info pinfo,char *name,char *grade,char *id);
int do_quit(struct pth_info pinfo,char *name);

int query_info(struct pth_info pinfo,char *id,char *grade);
int do_add_info(struct pth_info pinfo);
int do_del_info(struct pth_info pinfo);
int do_up_info(struct pth_info pinfo,char *id,char *grade);
int bind_info(struct pth_info pinfo,char *name,char *id);

int recv_data_from_client();
int send_data_to_client(int newfd,char type,char *buf,unsigned short num);


#endif 