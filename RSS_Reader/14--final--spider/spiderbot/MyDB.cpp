#include<iostream>  
#include<cstdlib> 
#include<string.h>
#include "MyDB.h"  
using namespace std;  
  
MyDB::MyDB()  
{  
    connection = mysql_init(NULL); // 初始化数据库连接变量  
    if(connection == NULL)  
    {  
        cout << "Error:" << mysql_error(connection);  
        exit(1);  
    }  
}  

string MyDB::escape(string str)
{
	//cout<<str.length()<<endl;
    //char to[4096] = {0};//字符串长度过小
	char to[2000000]={0};
    mysql_real_escape_string(connection,to, str.c_str(), str.length());//转化的是string，不是sql语句
	
	return to ;
}
 
MyDB::~MyDB()  
{  
    if(connection != NULL)  // 关闭数据库连接  
    {  
        mysql_close(connection);  
    }  
}  
  
bool MyDB::init(string host, string user, string pwd, string db_name)  
{  
    // 函数mysql_real_connect建立一个数据库连接  
    // 成功返回MYSQL*连接句柄，失败返回NULL  
    connection = mysql_real_connect(connection, host.c_str(),  
            user.c_str(), pwd.c_str(), db_name.c_str(), 0, NULL, 0);  
    if(connection == NULL)  
    {  
        cout << "Error:" << mysql_error(connection);  
        exit(1);  
    }  
    return true;  
}  
  
bool MyDB::exec_query(string sql)  
{  
    // mysql_query()执行成功返回0，失败返回非0值。与PHP中不一样  
    if(mysql_query(connection, sql.c_str()))  
    {  
        cout << "Query Error:" << mysql_error(connection);  
        exit(1);  
    }  
    else  
    {  
        /*
		result = mysql_use_result(connection); // 获取结果集  
        // mysql_field_count()返回connection查询的列数  
        for(int i=0; i < mysql_field_count(connection); ++i)  
        {  
            // 获取下一行  
            row = mysql_fetch_row(result);  
            if(row <= 0)  
            {  
                break;  
            }  
            // mysql_num_fields()返回结果集中的字段数  
            for(int j=0; j < mysql_num_fields(result); ++j)  
            {  
                cout << row[j] << " ";  
            }  
            cout << endl;  
        }  
        // 释放结果集的内存  
        */
		//result = mysql_use_result(connection);//结果需要一次性用完，否则会出错                                                       
		result = mysql_store_result(connection);//得到result是存下来的，无需把全部行结果读完
		while(row = mysql_fetch_row(result), row)
		{ 
			//cout<<row[0]<<"  "<<row[1]<<"  "<<row[2]<<endl;
			// mysql_num_fields()返回结果集中的字段数  
            for(int j=0; j < mysql_num_fields(result); ++j)  
            {  
                cout << row[j] << " ";  
            }  
            cout << endl;  
		}
		
		
        mysql_free_result(result);  
    }  
    return true;  
}

int MyDB::exec_update(string sql)//>0业务相关错误；<0业务无关错误；==0执行成功
{
	
	mysql_query(connection, "set names utf8") ;//设置编码格式，防止插入数据库中为乱码
	//int ret=mysql_query(connection, sql.c_str());
    int ret=mysql_real_query(connection, sql.c_str() ,sql.length());//??mysql_query() 在执行成功时返回 TRUE，出错时返回 FALSE。非 FALSE 的返回值意味着查询是合法的并能够被服务器执行。
    
	//cout<< ret<<endl;
	//cout<<mysql_errno(connection)<<endl;
	
	if(ret != 0)//mysql_query执行出错
	{
		if(mysql_errno(connection) == 1062)//重复
		{
			return 1062;
		}
		
		cout<<"exec_update:mysql_real_query failed, mysql_error:"<<mysql_error(connection)<<endl;
		return -1;
	}
	
	return 0;	
}
