#include "../include/mysqlite.h"


//创建数据库
int staff_sqlite_create(sqlite3 **db)
{
    //打开数据库
    if(sqlite3_open("./staff_system.db",db)!=0)
    {
        printf("ERROR: %s\n",sqlite3_errmsg(*db));
        return -1;
    }
    printf("数据库打开成功\n");
    return 0;
}


//执行语句
int execute_command(sqlite3 *db,char *list)
{
    char *errmsg=NULL;
    int res=0;
    res=sqlite3_exec(db,list,NULL,NULL,&errmsg);
    if(res!=SQLITE_OK)
    {
        printf("__%d__ERROR:%s\n",__LINE__,errmsg);
        return -1;
    }
    else
    {
        //printf("%d\n",res);
        //printf("命令执行成功成功\n");
        return 0;
    }
}
