#include "Queue.h"
#include "MyDB.h"//数据库操作
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

//1.检查提供的字符串是否为空
bool checkIfAllSpace(string content);

//2.解析绝对url并返回一个字符串数组包含域和文件名
char ** parseUrl(string url);

//3.获取来自服务器的html内容，并将内容作为字符串返回
string getPage(char domainName[100],char fileName[100]);

//4.解析html页面
void getTargetData(string content,string currentURL);


Queue * queue=new Queue();//待爬队列
Queue * crawledQueue=new Queue();//已爬队列

int main()
{
	string absoluteUrl;
	char ** pointer;
	Node * node;

	/*
	//rss数据读入方式一：手动插入队列
	node=new Node(1,"http://blog.sina.com.cn/rss/1286402547.xml","");
	queue->enqueue(node);
	node=new Node(2,"http://meiwenrishang.com/rss","");
	queue->enqueue(node);
	node=new Node(3,"http://feed.williamlong.info/","");
	queue->enqueue(node);
	*/
	
	
	//rss数据读入方式二：配置文件读入
	//1.打开配置文件
	FILE *fp = fopen("reader.conf", "r");
	if (fp == NULL)
	{
		printf("parseconf_load_file failed!!!");
		return 0;
	}
	//2.读取文件中的一行数据到setting_line,并处理
	char setting_line[1024] = {0};
	int index=1;//插入队列中节点node的id值
	while (fgets(setting_line, sizeof(setting_line), fp) != NULL)
	{
		if (strlen(setting_line) == 0 || setting_line[0] == '#')
			continue;
		//2.1 去除\r\n
		char *p = &setting_line[strlen(setting_line)-1];
		while (*p == '\r' || *p == '\n')
			*p-- = '\0';
		//2.2 将rss地址插入队列
		node=new Node(index,setting_line,"");
		queue->enqueue(node);
	
		memset(setting_line, 0, sizeof(setting_line));
		index++;
	}
	//3.关闭文件
	fclose(fp);
	/*4.打印从配置文件中，得到的url
	while(!queue->isEmpty())
	{
		node=queue->dequeue();
		absoluteUrl=node->getLink();
		cout<<"Current url: "<<endl<<absoluteUrl<<endl;
	}
	sleep(5);
	*/
	
	
	
	int i=1;
	while(!queue->isEmpty())//待爬队列非空
	{

		node=queue->dequeue();//1.从待爬队列出队
		crawledQueue->enqueue(node);//2.加入到已爬队列
		absoluteUrl=node->getLink();
		//cout<<endl<<"Current url: "<<endl<<absoluteUrl<<endl;

		if(checkIfAllSpace(absoluteUrl)==false)//3.判断url地址是否为空
		{
			pointer=parseUrl(absoluteUrl);//4.解析url地址到域和文件名
			if(pointer==NULL)
			{
				cout<<"无效的url地址"<<endl;
				continue;
			}
			string content=getPage(pointer[0],pointer[1]);//5.得到页面内容
			if(content.compare("--1ERROR")!=0)
			{
				getTargetData(content,absoluteUrl);//6.解析页面内容，并保存数据到mysql中
			}
			
		}
		i++;
	    cout<<"Num of nodes in the current queue: "<<queue->getNumOfNodes()<<endl;
		cout<<"Num of nodes in the crawled queue: "<<crawledQueue->getNumOfNodes()<<endl;
	}
	return 0;
}

bool checkIfAllSpace(string content)
{
	int state=0;
	int length=content.size();
	int i=0;
	while(i<length+1)
	{
		switch(state)
		{
			case 0:
				if(content[i]==' ')
				{
					state=0;
				}
				else
				{
					state=1;
				}
				break;
			case 1:
				return false;
				break;
			default:  
				return false; 
				break;
		}
		i++;
	}
	return true;
}

char ** parseUrl(string url)
{
	char ** pointer=new char *[2];
	int length=url.length(),pos=url.find_first_of("://");
	//cout<<endl<<"url in the parseUrl function: "<<url<<endl;//打印当前解析的url

	if(pos==-1)//字符串为空
	{
		pointer=NULL;
	}
	else
	{
		pos=pos+3;
		int pos1=url.find_first_of("/",pos);
		if(pos1==-1)//没有找到反斜线‘/’
		{
			int lengthOfDomain=length-pos;
			int lengthOfPath=1;
			string domain1,path1;
			domain1=url.substr(pos,lengthOfDomain);
			path1="/";
			char * domain=new char[domain1.size()+1];
			char *path=new char[path1.size()+1];
			strcpy(domain,domain1.c_str());
			strcpy(path,path1.c_str());

			pointer[0]=domain;
			pointer[1]=path;
		}
		else
		{
			int lengthOfDomain=(pos1-pos);
			int lengthOfPath=(length-pos1);
			string domain1,path1;//these are temporary variables

			domain1=url.substr(pos,lengthOfDomain);
			path1=url.substr(pos1,lengthOfPath);

			char * domain=new char[domain1.size()+1];
			char *path=new char[path1.size()+1];

			strcpy(domain,domain1.c_str());
			strcpy(path,path1.c_str());

			pointer[0]=domain;
			pointer[1]=path;
		}
	}
	return pointer;
}

string getPage(char * domainName,char *fileName)
{

	int s_id,zero=0, port_no=80, numberOfBytes=0, valopt,res;
	long arg;
	fd_set myset;
	struct timeval tv;
	socklen_t lon;
	string content;
	struct hostent *he;
	struct in_addr *addr;
	he = gethostbyname(domainName);
	if(he==NULL)
	{
		return "--1ERROR";
	}
	else
	{
		addr=(struct in_addr *)he->h_addr;
		char *address=(char *)inet_ntoa(*addr);
		ssize_t bytesRead;
		struct sockaddr_in serv_addr,remote_addr;
		int sin_size;
		
		s_id=socket(AF_INET,SOCK_STREAM,zero);
		serv_addr.sin_family=AF_INET;
		serv_addr.sin_port=htons(port_no);
		serv_addr.sin_addr.s_addr = inet_addr(address);
	
		//设置成非阻塞
		if( (arg = fcntl(s_id, F_GETFL, NULL)) < 0)
		{
			return "--1ERROR";
		}
		arg |= O_NONBLOCK;
		if( fcntl(s_id, F_SETFL, arg) < 0)
		{
			return "--1ERROR";
		}
		
		res = connect(s_id, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
		if (res < 0)
		{
			if (errno == EINPROGRESS)
			{
	
				do{
					tv.tv_sec = 15;
					tv.tv_usec = 0;
					FD_ZERO(&myset);
					FD_SET(s_id, &myset);
					res = select(s_id+1, NULL, &myset, NULL, &tv);
					if (res < 0 && errno != EINTR) 
					{
						return "--1ERROR";
					}
					else if (res > 0)
					{
						lon = sizeof(int);
						if (getsockopt(s_id, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
						{
							return "--1ERROR";
						}
						if (valopt)//检查返回值
						{
							return "--1ERROR";
						}
						break;
					}
					else
					{
						return "--1ERROR";
					} //end while
				} while (1);
			}
			else
			{
				return "--1ERROR";
			}
		}
		//设置回阻塞模式
		if( (arg = fcntl(s_id, F_GETFL, NULL)) < 0) 
		{
			return "--1ERROR";
		}
		arg &= (~O_NONBLOCK);
		if( fcntl(s_id, F_SETFL, arg) < 0) 
		{
			return "--1ERROR";
		}

		//char *data="GET / HTTP/1.1\nHost:www.mntechsolutions.net\n\n"; //i have to send all the messages as one message
		string data="GET ";
		char *charArrayData;

		data.append(fileName);
		data.append(" HTTP/1.1\nHost:");
		data.append(domainName);
		data.append("\nConnection: close\nCache-Control: max-age=0.0\nUser-Agent: SpiderBot\nAccept: link/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8,image/gif,image/jpeg,image/jpg\nAccept-Language: en-US,en;q=0.8\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\nContent-Transfer-Encoding: base64\n\n");//i am appending the new lines. the command has to end with two new lines \n\n

		int x=0;
		charArrayData=new char [data.size()+1];
		strcpy (charArrayData, data.c_str());

		if(send(s_id,charArrayData,data.size()+1,0)==-1)
		{
			return "--1ERROR";
		}
		FILE *p = NULL;
		char file[1000];
		char data1[1260]=" ";
		int done=0;
		char * val;
		char *val1="</html>";
		int i=0;
		char a;
		int counterStuck=0;

		fcntl(s_id,F_SETFL,O_NONBLOCK);
		while(done==0)
		{
			int re;
			while( (re=recv(s_id,data1,1024,0))==-1)
			{
				counterStuck++;
				if(counterStuck>10000000)
				{
					return "--1ERROR";
				}
			}
			content.append(data1,re);
			for(i=0;i<1260;i++)
			{
				data1[i]=' ';
			}
			numberOfBytes=numberOfBytes+re;
			
			if(recv(s_id,&a,1,MSG_PEEK | MSG_DONTWAIT)==0)//窥探数据是否读完
			{
				done=1;
			}
			else
			{
				done=0;
			}
		}
		close(s_id);
		//cout<<content;//打印请求页面的内容
		
		return content;
	}
}

void getTargetData(string content,string currentURL)
{
	string link,title,description;
	vector<string> titles;//文章标题
	vector<string> links;//文章链接
	vector<string> descriptions;//文章描述
	
	string link_channel,title_channel;//频道链接/频道标题
	
	int in_item=0;//是否在<item>中
	int length=content.length(),state=0,i=0;
	while(i<length)
	{
		switch(state)
		{
		case 0:
			if(content[i]=='<')
			{
				state=1;
			}
			else if(content[i]==' ')
			{
				state=0;
			}
			else
			{
				state=0;
			}
			break;

		case 1:
			if(content[i]=='l')
			{//start of the <link> tag
				state=57;
			}
			else if(content[i]=='t')
			{//start of the <title> tag
				state=79;
			}
			else if(content[i]=='d')
			{//start of the <description> tag
				state=85;
			}
			else if(content[i]=='i')
			{//start of the <item> tag
				state=110;
			}
			else if(content[i]=='/')
			{//start of the </item> tag
				state=114;
			}
			else
			{
				state=0;
			}
			break;

		case 57:
			if(content[i]=='i')
			{
				state=58;
			}
			else
			{
				state=0;
			}
			break;

		case 58:
			if(content[i]=='n')
			{
				state=59;
			}
			else
			{
				state=0;
			}

			break;

		case 59:
			if(content[i]=='k')
			{
				state= 60;
			}
			else
			{
				state=0;
			}
			break;

		case 60:
			if(content[i]=='>')//<link>
			{
				state=61;
			}
			else
			{
				state=60;
			}
			break;
			
		case 61:
			if(content[i]=='<')
			{
				if(in_item)
				{
					if(link.length()==0)
					{
						links.push_back("null");
					}
					else
					{
						links.push_back(link);
					}
				}
				else
				{
					link_channel=link;
				}
				link.clear();
				state=1;
             }
			else if(content[i]==' ')
			{
				state=61;
			}
			else
			{
				link.append(1,content[i]);
				state=61;
			}
			break;

		case 79:
			if(content[i]=='i')
			{
				state=80;
			}
			else
			{
				state=0;
			}
			break;
			
		case 80:
			if(content[i]=='t')
			{
				state=81;
			}
			else
			{
				state=0;
			}
			break;
			
		case 81:
			if(content[i]=='l')
			{
				state=82;
			}
			else
			{
				state=0;
			}
			break;

		case 82:
			if(content[i]=='e')
			{
				state=83;
			}
			else
			{
				state=0;
			}
			break;

		case 83:
			if(content[i]=='>')//<tilte>
			{
				state=84;	
			}
			else
			{
				state=83;
			}
			break;
		
		case 84:
			if(content[i]=='<')
			{
				if(in_item)
				{
					if(title.length()==0)
					{
						titles.push_back("null");
					}
					else
					{
						titles.push_back(title);
					}
				}
				else
				{
					title_channel=title;
				}
				title.clear();
				state=1;
			}
			else
			{
				
                title.append(1,content[i]);

				state=84;
			}
			break;
	
		case 85:
			if(content[i]=='e')
			{	
				state=86;
			}
			else
			{
				state=0;
			}
			break;
			
		case 86:
			if(content[i]=='s')
			{	
				state=87;
			}
			else
			{
				state=0;
			}
			break;
		
		case 87:
			if(content[i]=='c')
			{	
				state=88;
			}
			else
			{
				state=0;
			}
			break;
		
		case 88:
			if(content[i]=='r')
			{	
				state=89;
			}
			else
			{
				state=0;
			}
			break;
			
		case 89:
			if(content[i]=='i')
			{	
				state=90;
			}
			else
			{
				state=0;
			}
			break;
			
		case 90:
			if(content[i]=='p')
			{	
				state=91;
			}
			else
			{
				state=0;
			}
			break;
			
		case 91:
			if(content[i]=='t')
			{	
				state=92;
			}
			else
			{
				state=0;
			}
			break;
			
		case 92:
			if(content[i]=='i')
			{	
				state=93;
			}
			else
			{
				state=0;
			}
			break;
		
		case 93:
			if(content[i]=='o')
			{	
				state=94;
			}
			else
			{
				state=0;
			}
			break;
			
		case 94:
			if(content[i]=='n')
			{	
				state=95;
			}
			else
			{
				state=0;
			}
			break;
			
		case 95:
			if(content[i]=='>')//<description>
			{	
				state=96;
			}
			else
			{
				state=0;
			}
			break;
			
		case 96:
			if(content[i]=='<')
			{
				description.append(1,content[i]);
				state=97;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 97:
			if(content[i]=='/')
			{	
				description.append(1,content[i]);
				state=98;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 98:
			if(content[i]=='d')
			{	
				description.append(1,content[i]);
				state=99;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 99:
			if(content[i]=='e')
			{	
				description.append(1,content[i]);
				state=100;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 100:
			if(content[i]=='s')
			{	
				description.append(1,content[i]);
				state=101;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 101:
			if(content[i]=='c')
			{	
				description.append(1,content[i]);
				state=102;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 102:
			if(content[i]=='r')
			{	
				description.append(1,content[i]);
				state=103;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 103:
			if(content[i]=='i')
			{	
				description.append(1,content[i]);
				state=104;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 104:
			if(content[i]=='p')
			{	
				description.append(1,content[i]);
				state=105;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 105:
			if(content[i]=='t')
			{	
				description.append(1,content[i]);
				state=106;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 106:
			if(content[i]=='i')
			{	
				description.append(1,content[i]);
				state=107;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 107:
			if(content[i]=='o')
			{	
				description.append(1,content[i]);
				state=108;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 108:
			if(content[i]=='n')
			{	
				description.append(1,content[i]);
				state=109;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;
			
		case 109:
			if(content[i]=='>')//</description>
			{	
				description.append(1,content[i]);
				
				//去掉description尾部的</description>
				int len=description.length();
				string description_final;
				description_final.assign(description,0,len-14);
				
				if(in_item)
				{
					if(description_final.length()==0)
					{
						descriptions.push_back("null");
					}
					else
					{
						descriptions.push_back(description_final);
					}
				}
				description.clear();
				state=1;
			}
			else
			{
				description.append(1,content[i]);
				state=96;
			}
			break;	
			
		case 110:
			if(content[i]=='t')
			{	
				state=111;
			}
			else
			{
				state=0;
			}
			break;
			
		case 111:
			if(content[i]=='e')
			{	
				state=112;
			}
			else
			{
				state=0;
			}
			break;
			
		case 112:
			if(content[i]=='m')
			{	
				state=113;
			}
			else
			{
				state=0;
			}
			break;
			
		case 113:
			if(content[i]=='>')//<item>
			{	
				in_item=1;
				state=0;
			}
			else
			{
				state=0;
			}
			break;
			
		case 114:
			if(content[i]=='i')
			{
				state=115;
			}
			else
			{
				state=0;
			}
			break;
			
		case 115:
			if(content[i]=='t')
			{
				state=116;
			}
			else
			{
				state=0;
			}
			break;
			
		case 116:
			if(content[i]=='e')
			{
				state=117;
			}
			else
			{
				state=0;
			}
			break;
			
		case 117:
			if(content[i]=='m')
			{
				state=118;
			}
			else
			{
				state=0;
			}
			break;
			
		case 119:
			if(content[i]=='>')// end </item>
			{
				in_item=0;
				state=0;
			}
			else
			{
				state=0;
			}
			break;
						
		default:
			state=0;
			break;

		}//end switch
		i++;
	}//end while

	cout<<endl<<endl<<"【feed标题】"<<title_channel<<"   "<<"【url地址】"<<currentURL/*link_channel*/<<endl;//link_channel和currentURL不一样
	cout<<endl<<"article: "<<endl;
	int a;
	for(a=0;a<titles.size();a++)
	{
		cout<<titles[a]<<endl;
	}

	cout<<endl<<"links: "<<endl;
	for(a=0;a<links.size();a++)
	{
		cout<<links[a]<<endl;
	}

/*	
	cout<<endl<<"descriptions: "<<endl;
	for(a=0;a<descriptions.size();a++)
	{
		cout<<descriptions[a]<<endl;
		sleep(5);
	}//end for
	cout<<endl<<endl;
*/

	
	//写入数据库部分
	cout<<"write into mysql..."<<endl<<"<br>";
	
	//1.连接数据库
	MyDB db;
	db.init("localhost", "root", "123456", "spider");
	
	for(a=0;a<links.size() && a<titles.size() && a<descriptions.size();a++)
	{
		if(links[a].length()==0 || titles[a].length()==0 || descriptions[a].length()==0 || title_channel.length()==0)
		{
			continue;
		}
		
		//2.拼接sql语句
		std::ostringstream oss;//ignore
		oss << "insert into spider_rss set "  
			<< "Url='" << links[a] << "',"
			<< "Title='" << titles[a] <<"',"
			<< "Source='" << title_channel <<"',"
			<< "LinkChannel='" << currentURL <<"',"
			<< "Description='" << db.escape(descriptions[a])<< "'";

		//3.执行插入       
		int ret = db.exec_update(oss.str());
		if(ret < 0)//mysql api函数执行失败
		{
			cout<<"exec_update failed"<<endl;
			continue;
		}
		if(ret == 1062)
		{
			cout<<"重复插入"<<endl;
		}
		oss.clear();
	}
	//db.exec_query("select * from spider_rss");//查找成功（数据库内为乱码，读出后正确）
	
	cout<<"write ok!!!"<<endl<<"<br>";
}

