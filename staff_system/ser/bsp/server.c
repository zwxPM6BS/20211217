#include "../include/server.h"
#include "../include/mysqlite.h"

int network_init(int *sfd)
{
    //创建流式套接字
    *sfd=socket(AF_INET,SOCK_STREAM,0);
    if(*sfd<0)
    {
        perror("socket");
        return -1;
    }

    //允许端口快速重用
    int reuse = 1;
    if(setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &reuse , sizeof(int)) < 0)
    {
        ERR_MSG("setsockopt");
        return -1;
    }
    //设置服务器的ip和端口
    //填充地址信息结构体
    struct sockaddr_in sin;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);   //服务器主机端口的网络字节序
    sin.sin_addr.s_addr = inet_addr(IP); //服务器ip地址的网络字节序

    if(bind(*sfd,(struct sockaddr *)&sin,sizeof(sin))<0)
    {
        ERR_MSG("bing");
        return -1;
    }
    //将套接字设置成被动监听状态
    if(listen(*sfd,10)<0)
    {
        ERR_MSG("listen");
        return -1;
    }
    printf("监听成功\n");
    return 0;
}

//多线程并发服务器
int multithreaded_server(int sfd,sqlite3 *db)
{
    struct pth_info pinfo;
    memset(&pinfo,0,sizeof(pinfo));
    pinfo.newfd=0;
    pinfo.db=db;

    socklen_t addrlen =sizeof(pinfo.cin);
    pthread_t tid;

    while(1)
    {
        //等待客户端连接
        pinfo.newfd=accept(sfd,(struct sockaddr *)&pinfo.cin,&addrlen);
        if(pinfo.newfd<0)
        {
            ERR_MSG("accept");
            return -1;
        }
        printf("连接成功\n");

        //创建分支线程
        if(pthread_create(&tid,NULL,RcvCliMsg,(void *)&pinfo)!=0)
        {
            ERR_MSG("pthread_create");
            return -1;
        }
    }
}

//回调函数
void *RcvCliMsg(void *arg)
{
    //线程分离
    pthread_detach(pthread_self());

    struct pth_info pinfo=*(struct pth_info *)arg;
    int newfd=pinfo.newfd;
    struct sockaddr_in cin=pinfo.cin;
    char name[64]="";
    char grade[64]="";
    char id[10]="";

    char *Cli_ip=inet_ntoa(cin.sin_addr);   //客户端的ip地址
    uint16_t Cli_port=ntohs(cin.sin_port);  //客户端的端口号
    printf("%s[%d]:链接服务器\n",Cli_ip,Cli_port);

    while(1)
    {
        memset(&pinfo.data_p,0,sizeof(pinfo.data_p));
        if(recv(newfd,&pinfo.data_p,sizeof(pinfo.data_p),0)<0)
        {
            ERR_MSG("recv");
            continue;
        }
        switch(pinfo.data_p.type)
        {
            case 'R':
                //注册
               do_regist(pinfo);
                break;
            case 'L':
                //登录
                memset(name,0,sizeof(name));
                memset(grade,0,sizeof(grade));
                memset(id,0,sizeof(id));
                do_login(pinfo,name,grade,id);
                break;
            case 'T':
                //绑定工号
                bind_info(pinfo,name,id);
                break;
            case 'C':
                //查询信息
                query_info(pinfo,id,grade);
                break;
            case 'P':
                //添加信息
                do_add_info(pinfo);
                break;
            case 'D':
                //删除信息
                do_del_info(pinfo);
                break;
            case 'U':
                //修改信息
                do_up_info(pinfo,id,grade);
                break;
            case 'B':
                //退出登录
                do_quit(pinfo,name);
                memset(name,0,sizeof(name));
                memset(grade,0,sizeof(grade));
                memset(id,0,sizeof(id));
                break;
            case 'Q':
                //退出客户端
                do_quit(pinfo,name);
                memset(name,0,sizeof(name));
                memset(grade,0,sizeof(grade));
                memset(id,0,sizeof(id));

                char *Cli_ip=inet_ntoa(pinfo.cin.sin_addr);   //客户端的ip地址
                uint16_t Cli_port=ntohs(pinfo.cin.sin_port);  //客户端的端口号
	            printf("%s[%d:断开链接\n",Cli_ip,Cli_port);

                close(pinfo.newfd);
                pthread_exit(NULL);
            default:
                printf("参数错误\n");
                break;
        }
    }
    close(pinfo.newfd);
    pthread_exit(NULL);
}

//绑定id
int bind_info(struct pth_info pinfo,char *name,char *id)
{
    int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

	char temp[128]="";
	bzero(temp,128);
	sprintf(temp,"select * from staff where Name='%s' and Id='%s'",data_p.user,data_p.id);
	char** result = NULL;
	int row, column;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,temp, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		printf("__%d__ERROR:%s\n",__LINE__,errmsg);
		return -1;
	}
	//判断	
	if(0==row)
	{
        send_data_to_client(newfd,'E',"信息错误",0);
	}
	else
	{
        bzero(temp,128);
        sprintf(temp,"update passwad set id='%s' where user='%s'",data_p.id,name);
        //printf("%s\n",temp);
        if(0==execute_command(db,temp))
        {
            bzero(id,10);
            strcpy(id,data_p.id); //工号
            send_data_to_client(newfd,'A',"绑定成功",0);
        }
	}
	sqlite3_free_table(result);
    return 1;
}


//客户端注册
int do_regist(struct pth_info pinfo)
{
	unsigned short num=0;
	int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

	int res=0;

	//记录账号密码	
	char flag='n';
    char id='0';
	char temp[256]="";
	bzero(temp,256);
	sprintf(temp, "insert into passwad values(\"%s\",\"%s\",\"%s\",'%c','%c')",data_p.user,data_p.id,data_p.buf,flag,id);
	char *errmsg = NULL;
	res=sqlite3_exec(db,temp, NULL, NULL, &errmsg);
	if(res!= SQLITE_OK)
	{
		if(SQLITE_CONSTRAINT==res)
		{
            send_data_to_client(newfd,'E',"注册失败,账号已存在",0);
		}
		else 
		{
			printf("__%d__ERROR:%s\n",__LINE__,errmsg);
			return -1;
		}
	}
	else
	{
        send_data_to_client(newfd,'A',"注册成功",0);
    }
	return 1;
}

//客户端登录
int do_login(struct pth_info pinfo,char *name,char *grade,char *id)
{
	int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

	//查询已登入信息
	//flag位 n:无登入 y:已登入
	char temp[128]="";
	bzero(temp,128);
	sprintf(temp,"select * from passwad where user='%s'",data_p.user);
	char** result = NULL;
	int row, column;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,temp, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		printf("__%d__ERROR:%s\n",__LINE__,errmsg);
		return -1;
	}
	//判断登入情况	
	if(0==row)
	{
        send_data_to_client(newfd,'E',"用户名不存在",0);
	}
	else if(strcmp(result[8],"n")==0)
	{
		if(strcmp(result[6],data_p.id)==0)
		{
			bzero(temp,128);
			sprintf(temp,"update passwad set flag='y' where user='%s'",data_p.user);
			errmsg= NULL;
			if(sqlite3_exec(db,temp,NULL, NULL, &errmsg)!=0)
			{
				printf("__%d__ERROR:%s\n",__LINE__,errmsg);
				return -1;
			}

			//printf("%s[%d]<%s>登入成功\n",Cli_ip,Cli_port,data_p.user);
            strcpy(grade,result[7]); //权限
            bzero(temp,128);
            strcpy(temp,"登入成功");
            strcpy(temp+64,result[7]);
            strcpy(name,data_p.user); //账号号
            strcpy(id,result[9]);   //工号
            if(strcmp(id,"0")!=0)
            {
                strcpy(temp+100,"bind");  
            }
            send_data_to_client(newfd,'A',temp,0);
		}
		else 
		{
            bzero(temp,sizeof(temp));
            strcpy(temp,"密码错误");
            send_data_to_client(newfd,'E',temp,0);
		}
	}
	else
	{
        send_data_to_client(newfd,'E',"登入失败,账号已被登入",0);
	}
	sqlite3_free_table(result);
	return 1;
}

//添加信息
int do_add_info(struct pth_info pinfo)
{
    //printf("__%d__\n",__LINE__);
    unsigned short num=1;
	int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

    char temp[516]="";
	bzero(temp,sizeof(temp));
    sprintf(temp,"insert into staff(Name,Sex,Age,Address,id,Phone) values (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",\
                                     data_p.user,data_p.buf,data_p.buf+10,data_p.buf+50,data_p.id,data_p.buf+20);
	if(0==execute_command(db,temp))
    {
        send_data_to_client(newfd,'A',"添加信息成功",num);
    }
    else
    {
        send_data_to_client(newfd,'E',"添加信息失败",num);
    }
    return 1;
}

//删除信息
int do_del_info(struct pth_info pinfo)
{
    unsigned short num=1;
	int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

    char temp[128]="";
	bzero(temp,sizeof(temp));
    sprintf(temp,"delete from staff where name='%s' and Id='%s'",data_p.user,data_p.id);
	if(0==execute_command(db,temp))
    {
        bzero(temp,sizeof(temp));
        sprintf(temp,"update passwad set id='0' where id='%s'",data_p.id);
        execute_command(db,temp);
        send_data_to_client(newfd,'A',"删除信息成功",num);
    }
    else
    {
		//printf("%d\n",__LINE__);
        send_data_to_client(newfd,'E',"删除信息失败",num);
    }
    return 1;
}

//修改信息
int do_up_info(struct pth_info pinfo,char *id,char *grade)
{
    int i=0;
    unsigned short num=0;
    int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

    char temp[256]="";

    if(htons(num)!=*(unsigned short *)(&data_p.pos))
    {
        return -1;
    }

    if(strcmp(id,data_p.id)!=0&&strcmp(grade,"admin")!=0)
    {
        bzero(temp,sizeof(temp));
        strcpy(temp,"没有权限");
        num++;
        send_data_to_client(newfd,'E',temp,num);
        return -1;
    }
    bzero(temp,sizeof(temp));
    sprintf(temp,"select * from staff where Name='%s' and Id='%s' ",data_p.user,data_p.id);

    char** result = NULL;
	int row=0, column=0;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,temp, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		printf("__%d__ERROR:%s\n",__LINE__,errmsg);
		return -1;
	}
	if(0==row)
	{
		num++;
        bzero(temp,sizeof(temp));
        strcpy(temp,"无此员工");
        send_data_to_client(newfd,'E',temp,num);
	}
	else
	{
        for(i=0;i<6;i++)
        {
            if((result[8+i]==NULL)||(strcmp(result[8+i],"NULL")==0))
            {
                num++;
                bzero(temp,sizeof(temp));
                strcpy(temp,"NULL");
                send_data_to_client(newfd,'O',temp,num);  
            }
            else
            {
                num++;
                send_data_to_client(newfd,'O',result[8+i],num);
            }
            if(recv(newfd,&data_p,sizeof(data_p),0)<0)
            {
                ERR_MSG("recv");
                return -1;
            }
            //printf("__%d__\n",__LINE__);
            if(htons(num)!=*(unsigned short *)(&data_p.pos))
            {
                return -1;
            }
        }
        bzero(temp,sizeof(temp));
        strcpy(temp,"传送完毕");
        num++;
        send_data_to_client(newfd,'A',temp,num);

        //printf("%d\n",__LINE__);

        if(recv(newfd,&data_p,sizeof(data_p),0)<0)
        {
            ERR_MSG("recv");
            return -1;
        }
        bzero(temp,sizeof(temp));
        sprintf(temp,"delete from staff where Id='%s'",result[7]);
        //printf("%s\n",temp);
        if(0==execute_command(db,temp))
        {
            bzero(temp,sizeof(temp));
            sprintf(temp,"insert into staff values('%s','%s','%s','%s','%s','%s','%s')",\
                                    result[7],data_p.user,data_p.buf,data_p.buf+10,data_p.buf+20,data_p.buf+40,data_p.buf+50);
            //printf("%s\n",temp);
            execute_command(db,temp);
            bzero(temp,sizeof(temp));
            strcpy(temp,"修改信息完成");
            send_data_to_client(newfd,'E',temp,num);
        }
        else
        {
            bzero(temp,sizeof(temp));
            strcpy(temp,"修改信息失败");
            send_data_to_client(newfd,'E',temp,num);
        }
    }
    sqlite3_free_table(result);
    return 1;
}

//-------------------------------------------------------------------------------------------------------------------------------------------

//查询信息
int query_info(struct pth_info pinfo,char *id,char *grade)
{
	int i=0;
    char nid[10]={0};
	unsigned short num=0;
	int newfd=pinfo.newfd;
	struct mess data_p=pinfo.data_p;
	sqlite3 *db=pinfo.db;

    strcpy(nid,data_p.id);
	char temp[128];
	bzero(temp,128);
    //sprintf(temp,"select * from staff where Id='%s' ",data_p.id);

    if(strcmp(data_p.id,"all")==0&&strcmp(grade,"admin")==0)
    {
        sprintf(temp,"select * from staff"); //查看所有
    }
    else
    {
        sprintf(temp,"select * from staff where Name='%s' and Id='%s' ",data_p.user,data_p.id); //查询
    }
	char** result = NULL;
	int row=0, column=0;
	char *errmsg = NULL;
	if(sqlite3_get_table(db,temp, &result, &row, &column, &errmsg) != SQLITE_OK)
	{
		printf("__%d__ERROR:%s\n",__LINE__,errmsg);
		return -1;
	}
	if(0==row)
	{
		num++;
        bzero(temp,sizeof(temp));
        strcpy(temp,"无此员工");
        send_data_to_client(newfd,'E',temp,num);
	}
	else
	{
		for(i=0;i<row+1;i++)
		{
			if(htons(num)==*(unsigned short *)(&data_p.pos))
			{
				bzero(temp,sizeof(temp));
                if(strcmp(grade,"admin")==0||strcmp(id,nid)==0)
                {
                    sprintf(temp,"%5s|  %6s|  %3s| %4s|  %20s|  %8s| %13s",result[7*i],\
                                                result[7*i+1],result[7*i+2],result[7*i+3],result[7*i+4],result[7*i+5],result[7*i+6]);
                }
                else
                {
                    if(i==0)
                    {
                        sprintf(temp,"%5s|  %6s|  %3s| %4s|  %20s|  %8s| %13s",result[7*i],\
                                                result[7*i+1],result[7*i+2],result[7*i+3],result[7*i+4],result[7*i+5],result[7*i+6]);
                    }
                    else
                    {
                        sprintf(temp,"%5s|  %6s|  %3s| %4s|  ********************|  ******| %13s",result[7*i],\
                                                result[7*i+1],result[7*i+2],result[7*i+3],result[7*i+6]);
                    }
                }
				num++;
                send_data_to_client(newfd,'A',temp,num);
				do
				{
					if(recv(newfd,&data_p,sizeof(data_p),0)<0)
					{
						ERR_MSG("recv");
						return -1;
					}
				}while(data_p.type!='C');
			}
		}
		//发送查询完毕信息
        num++;
        bzero(temp,sizeof(temp));
        strcpy(temp,"查询完毕");
        send_data_to_client(newfd,'E',temp,num);
	}
	sqlite3_free_table(result);
	return -1;
}


//退出登录
int do_quit(struct pth_info pinfo,char *name)
{
	sqlite3 *db=pinfo.db;
	char temp[128]="";
	bzero(temp,128);
    sprintf(temp,"update passwad set flag='n' where user='%s'",name);
	char *errmsg = NULL;
	if(sqlite3_exec(db,temp,NULL, NULL, &errmsg)!=0)
	{
		printf("__%d__ERROR:%s\n",__LINE__,errmsg);
		return -1;
	}

	return 1;
}

//接收数据
int recv_data_from_client()
{
    return 1;
}
//发送数据
int send_data_to_client(int newfd,char type,char *buf,unsigned short num)
{
    struct mess data_p;
    memset(&data_p,0,sizeof(struct mess));
    *(unsigned short *)(&data_p.pos)=htons(num);
    data_p.type=type;
    strcpy(data_p.buf,buf);
    strcpy(data_p.buf+64,buf+64);
    strcpy(data_p.buf+100,buf+100);
    if(send(newfd,&data_p,sizeof(data_p),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
    return 1;
}
