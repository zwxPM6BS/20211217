#include "../include/client.h"
#include "../include/viewer.h"


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

    //连接到指点服务器
    struct sockaddr_in serin;        //服务器的IP地址和端口号
    serin.sin_family=AF_INET;        
    serin.sin_port=htons(PORT);            //服务器端口
    serin.sin_addr.s_addr=inet_addr(IP);   //服务器ip

    //连接服务器
    if(connect(*sfd,(struct sockaddr *)&serin,sizeof(serin))<0)
    {
        ERR_MSG("connect");
        return -1;
    }
    printf("连接成功\n");
    return 1;
}


int recv_data_from_server(int sfd,struct mess *data_p,unsigned short num)
{

    memset(data_p, 0, sizeof(struct mess));
	if(recv(sfd,data_p,sizeof(struct mess),0)<0)
	{
		ERR_MSG("recv");
		return -1;
	}
    if(htons(num)==*(unsigned short *)(&data_p->pos))
    {
        if('A'==data_p->type)
        {
            printf("%s\n",data_p->buf);
            return 0;
        }
        else if ('E'==data_p->type)
        {
            printf("%s\n",data_p->buf);
            return -2;
        }
        else if('O'==data_p->type)
        {
            return 2;
        }
    }
    return 1;
}

int send_data_to_server(int sfd,char type,unsigned short num)
{
    struct mess data_p;
    memset(&data_p,0,sizeof(data_p));
    *(unsigned short *)(&data_p.pos)=htons(num);
    data_p.type=type;
    if(type=='R'||type=='L')
    {
        printf("请输入账号>>>");
        scanf("%s",data_p.user);
        getchar();
        printf("请输入密码>>>");
        scanf("%s",data_p.id);
        getchar();
        if(type=='R')
        {
            printf("请输入验证码>>>");
            scanf("%s",data_p.buf);
            getchar();
        }
    }
    else if((type == 'C'||type == 'D'||type == 'U')&&num==0)
    {
        printf("请输入员工姓名>>>");
        scanf("%s",data_p.user);
        getchar();
        printf("请输入员工工号>>>");
        scanf("%s",data_p.id);
        getchar();
    }
    else if(type=='P')
    {
        printf("请输入员工姓名>>>");
        scanf("%s",data_p.user);
        getchar();
        printf("请输入员工工号>>>");
        scanf("%s",data_p.id);
        getchar();
        printf("请输入员工性别>>>");
        scanf("%s",data_p.buf);
        getchar();
        printf("请输入员工年龄>>>");
        scanf("%s",data_p.buf+10);
        getchar();
        printf("请输入员工电话>>>");
        scanf("%s",data_p.buf+20);
        getchar();
        printf("请输入员工住址>>>");
        scanf("%s",data_p.buf+50);
        getchar();
    }
    else if(data_p.type =='T')
    {
        printf("输入员工姓名>>>");
        scanf("%s",data_p.user);
        getchar();
        printf("输入员工工号>>>");
        scanf("%s",data_p.id);
        getchar();
        //printf("__%d__\n",__LINE__);
    }
    //发送请求
    if(send(sfd,&data_p,sizeof(data_p),0)<0)
    {
        ERR_MSG("send");
        return -1;
    }
    return 0;
}
