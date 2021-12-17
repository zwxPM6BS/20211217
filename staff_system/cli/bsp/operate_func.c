#include "../include/operate_func.h"
#include "../include/viewer.h"
#include "../include/client.h"

//注册账号
int do_regist(int sfd)
{
    struct mess data_p;
	//输入注册账号密码
	send_data_to_server(sfd,'R',0);
    
	//等待接收注册结果
	recv_data_from_server(sfd,&data_p,0);
	return 1;
}

//登录账号
int do_login(int sfd)
{
	//输入登入账号密码
	 struct mess data_p;
	 memset(&data_p,0,sizeof(struct mess));
	 send_data_to_server(sfd,'L',0);
	
	//等待接收登入结果
	if(0==recv_data_from_server(sfd,&data_p,0))
	{
		if(strcmp(data_p.buf+64,"admin")==0)
		{
			admin_viewer(sfd);         //进入管理员界面
		}
		else
		{
			if(strcmp(data_p.buf+100,"bind")!=0)
			{
				if(bind_viewer(sfd)>0)
				{
					staff_viewer(sfd);          //进入员工界面
				}
			}
			else
				staff_viewer(sfd);          //进入员工界面
		}
	}
	return 1;
}


//绑定工号
int bing_info(int sfd)
{
	 struct mess data_p;
	send_data_to_server(sfd,'T',0);
	if(0==recv_data_from_server(sfd,&data_p,0))
	{
		return 1;
	}
	return -1;
}

//添加信息
int add_info(int sfd)
{
	struct mess data_p;
	memset(&data_p,0,sizeof(data_p));
	unsigned short num=0;
	send_data_to_server(sfd,'P',num);
	num++;
	//等待接收查询结果
	recv_data_from_server(sfd,&data_p,num);
	return 1;
}


//删除信息
int del_info(int sfd)
{
	struct mess data_p;
	memset(&data_p,0,sizeof(data_p));
	unsigned short num=0;
	send_data_to_server(sfd,'D',num);
	num++;
	//等待接收查询结果
	recv_data_from_server(sfd,&data_p,num);
	return 1;
}


//查询信息
int query_info(int sfd)
{
	int res;
	struct mess data_p;
	memset(&data_p,0,sizeof(data_p));
	unsigned short num=0;
	send_data_to_server(sfd,'C',num);
	num++;
	//等待接收查询结果
	while(1)
	{
		res=recv_data_from_server(sfd,&data_p,num);
		if (res==0)
		{
			send_data_to_server(sfd,'C',num);
			num++;
		}
		else
		{
			return 1;
		}
	}
}

//修改信息
int modify_info(int sfd,int grade)
{
	char info[6][30];
	memset(info,0,sizeof(info));
	int i=0,res=0;
	unsigned short num=0;
	struct mess data_p;
	memset(&data_p,0,sizeof(data_p));

	send_data_to_server(sfd,'U',num);
	num++;
	while(1)
	{
		//printf("%d\n",__LINE__);
		res=recv_data_from_server(sfd,&data_p,num);
		if(res<0)
		{
			return -1;
		}
		else if(res==0)
		{
			break;
		}
		else if(res==2)
		{
			strcpy(info[i],data_p.buf);
			i++;
			send_data_to_server(sfd,'U',num);
			num++;
		}
	}
	modify_viewer(info,&data_p,grade);
	data_p.type='U';
	*(unsigned short *)(&data_p.pos)=htons(num);
	if(send(sfd,&data_p,sizeof(data_p),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
	num++;
	//等待接收查询结果
	recv_data_from_server(sfd,&data_p,num);
	return 1;
}

int do_break(int sfd)
{
	send_data_to_server(sfd,'Q',0);
	return 1;
}
//退出登录
int do_quit(int sfd)
{
	//printf("退出程序中.....\n");
	char type='B';
	send_data_to_server(sfd,type,0);
	return 1;
}
