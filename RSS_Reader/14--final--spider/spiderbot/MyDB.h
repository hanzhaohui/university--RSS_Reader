#ifndef __MYBD_H__  
#define __MYBD_H__
#include<iostream>  
#include<string>  
#include<mysql/mysql.h>  
using namespace std;  
  
class MyDB  
{  
public:  
    MyDB();  
    ~MyDB();  
    bool init(string host, string user, string pwd, string db_name);//连接数据库  
    bool exec_query(string sql);//查询
	int exec_update(string sql);//插入、删除、更新
	string escape(string str);//字符串转义
private:  
    MYSQL *connection;  
    MYSQL_RES *result;  
    MYSQL_ROW row;  
};  
  
#endif  
