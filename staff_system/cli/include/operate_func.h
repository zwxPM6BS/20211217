#ifndef __OPERATE_FUNC_H__
#define __OPERATE_FUNC_H__

#include "public.h"


int do_regist(int sfd);
int do_login(int sfd);

int add_info(int sfd);
int del_info(int sfd);
int query_info(int sfd);
int modify_info(int sfd,int grade);

int do_quit(int sfd);
int do_break(int sfd);
int bing_info(int sfd);

#endif