#include "../include/viewer.h"
#include "../include/client.h"
#include "../include/operate_func.h"

//登录注册界面
int first_viewer(int sfd)
{
    char choose;
	while(1)
	{
		system("clear");
		printf("************************************\n");
		printf("***************1:注册 **************\n");
		printf("***************2:登录 **************\n");
		printf("***************0:退出 **************\n");
		printf("************************************\n");
		printf("请输入选择>>>");
		scanf("%c",&choose);
		while(getchar()!=10);
		switch(choose)
		{
            case '1':
                do_regist(sfd);
                break;
            case '2':
            	do_login(sfd);
                break;
            case '0':
            	do_break(sfd);
                return 0;
            default:
                printf("输入有误,请重输\n");
                break;
		}
		printf("输入任意字符清屏\n");
		while(getchar()!=10);
	}
	return -1;
}

int bind_viewer(int sfd)
{
	char choose;
	while(1)
	{
		system("clear");
		printf("***********员 工 绑 定 界 面**********\n");
		printf("*************1:绑定工号*************\n");
		printf("*************0:退出登录*************\n");
		printf("************************************\n");
		printf("请输入选择>>>");
		scanf("%c",&choose);
		while(getchar()!=10);
		switch(choose)
		{
		case '1':
			if(bing_info(sfd)>0)
				return 1;
			else
				break;
		case '0':
			do_quit(sfd);
			system("clear");
			return -1;
		default:
			printf("输入有误,请重输\n");
			break;
		}
		printf("输入任意字符清屏\n");
		while(getchar()!=10);
	}
	return 0;
}

//员工界面
int staff_viewer(int sfd)
{
	char choose;
	while(1)
	{
		system("clear");
		printf("***********员 工 查 询 界 面**********\n");
		printf("*************1:查询信息*************\n");
		printf("*************2:修改信息*************\n");
		printf("*************0:退出登录*************\n");
		printf("************************************\n");
		printf("请输入选择>>>");
		scanf("%c",&choose);
		while(getchar()!=10);
		switch(choose)
		{
		case '1':
			query_info(sfd);
			break;
		case '2':
			modify_info(sfd,0);
			break;
		case '0':
			do_quit(sfd);
			system("clear");
			return -1;
		default:
			printf("输入有误,请重输\n");
			break;
		}
		printf("输入任意字符清屏\n");
		while(getchar()!=10);
	}
	return -1;
}

//管理员界面
int admin_viewer(int sfd)
{
	char choose;
	while(1)
	{
		system("clear");
		printf("***********管 理 查 询 界 面**********\n");
		printf("*************1:增加信息*************\n");
		printf("*************2:删除信息*************\n");
		printf("*************3:修改信息*************\n");
		printf("*************4:查询信息*************\n");
		printf("*************0:退出登录*************\n");
		printf("************************************\n");
		printf("请输入选择>>>");
		scanf("%c",&choose);
		while(getchar()!=10);
		switch(choose)
		{
			case '1':
				add_info(sfd);
				break;
			case '2':
				del_info(sfd);
				break;
			case '3':
				modify_info(sfd,1);
				break;
			case '4':
				query_info(sfd);
				break;
			case '0':
				do_quit(sfd);
				system("clear");
				return -1;
			default:
				printf("输入有误,请重输\n");
				break;
		}
		printf("输入任意字符清屏\n");
		while(getchar()!=10);
	}
	return -1;
}

int modify_viewer(char info[6][30],struct mess *data_p,int grade)
{
	char temp[128]="";
	
	printf("***********输入#表示不修改**********\n");

	printf("请输入员工姓名[%s]>>>",info[0]);
	bzero(temp, sizeof(temp));
	scanf("%s",temp);
	if(temp[0]!='#'){
		strcpy(data_p->user,temp);
		printf("1");}
	else{
		strcpy(data_p->user,info[0]);
		printf("2");}
	
	printf("请输入员工性别[%s]>>>",info[1]);
	bzero(temp, sizeof(temp));
	scanf("%s",temp);
	if(temp[0]!='#')
		strcpy(data_p->buf,temp);
	else
		strcpy(data_p->buf,info[1]);

	printf("请输入员工年龄[%s]>>>",info[2]);
	bzero(temp, sizeof(temp));
	scanf("%s",temp);
	if(temp[0]!='#')
		strcpy(data_p->buf+10,temp);
	else
		strcpy(data_p->buf+10,info[2]);
	
	printf("请输入员工住址[%s]>>>",info[3]);
	bzero(temp, sizeof(temp));
	scanf("%s",temp);
	if(temp[0]!='#')
		strcpy(data_p->buf+20,temp);
	else
		strcpy(data_p->buf+20,info[3]);

	if(grade==1)
	{
		printf("请输入员工工资[%s]>>>",info[4]);
		bzero(temp, sizeof(temp));
		scanf("%s",temp);
		if(temp[0]!='#')
			strcpy(data_p->buf+40,temp);
		else
			strcpy(data_p->buf+40,info[4]);
	}
	else
	{
		strcpy(data_p->buf+40,info[4]);
	}

	printf("请输入员工电话[%s]>>>",info[5]);
	bzero(temp, sizeof(temp));
	scanf("%s",temp);
	if(temp[0]!='#')
		strcpy(data_p->buf+50,temp);
	else
		strcpy(data_p->buf+50,info[5]);
	return 1;
}