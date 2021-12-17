#include "include/public.h"
#include "include/client.h"
#include "include/viewer.h"
//#include "include/operate_func.h"

//主函数
int main(int argc, const char *argv[])
{
	//网络初始化
	int sfd=0;
	if(network_init(&sfd)<0)
	{
		return -1;
	}

	//注册登录界面
	first_viewer(sfd);

	//程序结束
	close(sfd);
	return 0;
}

