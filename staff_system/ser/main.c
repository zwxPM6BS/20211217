#include"./include/mysqlite.h"
#include"./include/server.h"

int main(int argc, char const *argv[])
{
    sqlite3 *db=NULL;
    //创建数据库
    staff_sqlite_create(&db);
    execute_command(db,STAFF_LIST);
    execute_command(db,USER_LIST);

    //初始化服务器
    int sfd=0;
    network_init(&sfd);
    
    //连接客户端
    multithreaded_server(sfd,db);
 
    close(sfd);
    sqlite3_close(db);
    return 0;
}
