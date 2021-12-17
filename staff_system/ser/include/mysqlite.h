#ifndef  __MYSQLITE_H__
#define __MYSQLITE_H__

#include "public.h"


#define STAFF_LIST "create table if not exists staff(Id char primary key,Name char, Sex char,age char,Address char,Salary char,Phone char)"
#define USER_LIST "create table if not exists passwad(user char primary key,password char,type char,flag char,id char)"


int staff_sqlite_create(sqlite3 **db);
int execute_command(sqlite3 *db,char *list);

#endif