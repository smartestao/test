#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <thread>
#include <sstream>
#include <errno.h>
#include "cws_string.h"
#include "cws_file.h"
#include "CwebServErrno.h"
#include "CwebServHandler.h"
#include "thread_pool.h"
using namespace std;
#define QUANTITY 20000
#define QUANTITY_pool 500
#define QUANTITY_epoll 1000
#define epoll_timeout -1
//tail是url，data是请求数据，type是数据类型，raw是原始的，datas是&分割的那种数据结构的数据，下面两个vector是multiple那种的，一项是vector的一项
//method是请求类型，headers是头cookie是解析出来的cookie
#define WEB_APP int sockConn, string tail, body_content data, string method, map<string, string> headers, map<string, string> cookie
#define SOCKET_APP int sockConn, string tail, string message, map<string, string> headers, map<string, string> cookie, int event

std::string getThreadIdOfString(const std::thread::id &id)
{
	std::stringstream sin;
	sin << id;
	return sin.str();
}

class CwebServ
{
public:
	//addr_s 地址
	//p0rt 端口
	CwebServ(string addr_s, int p0rt)
	{
		SERVER_ADDRESS = addr_s;
		PORT = p0rt;
		set_port_sddr = 1;
	}
	//路由
	//route_ 要匹配的东西
	//app 处理函数
	//type 1：一模一样才行 2：匹配前缀，后面必须有num项 3：匹配前缀（大概，自己试试
	template <typename F>
	void add_app(string route_, F &&app, int type, int num = 0)
	{
		web_app adding;
		adding.app_ = app;
		Route.insert(route_, adding, type, num);
	}
	//开启服务
	void start()
	{
		if (set_port_sddr == 0)
		{
			PORT = 80;
			SERVER_ADDRESS = "0.0.0.0";
		}
		ThreadPool pool(QUANTITY_pool);
		cout << "running at " << SERVER_ADDRESS << ":" << PORT << endl;
		errno_cheek();
		serverSock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr(SERVER_ADDRESS.c_str());
		int SO_REUSEADDR_state = 1, alive = 1;
		setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &SO_REUSEADDR_state, sizeof(SO_REUSEADDR_state));
		setsockopt(serverSock, SOL_SOCKET, SO_KEEPALIVE, &alive,sizeof(alive));
		cout << "finish init socket" << endl;
		epollfd = epoll_create(QUANTITY_epoll);
		epoll_event epoll_event_serv;
		epoll_event_serv.events = EPOLLIN | EPOLLET;
		epoll_event_serv.data.fd = serverSock;
		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSock, &epoll_event_serv) == -1)
		{
			errno_cheek();
			return;
		}
		cout << "finish init epoll" << endl;
		if (bind(serverSock, (sockaddr *)&server, sizeof(server)) == -1)
		{
			errno_cheek();
			return;
		}
		if (listen(serverSock, QUANTITY) == -1)
		{
			errno_cheek();
			return;
		}
		cout << "start listening..." << endl;
		response_headers["type"] = "HTTP/1.1";
		response_headers["code"] = "200 OK";
		response_headers["content-type"] = "text/html; charset=utf-8";
		while (1)
		{
			int epoll_ret = epoll_wait(epollfd, epoll_event_list, QUANTITY_epoll, epoll_timeout);
			if (epoll_ret <= 0)
			{
				errno_cheek();
				continue;
			}
			for (int i = 0; i < epoll_ret; i++)
			{
				if ((epoll_event_list[i].events & EPOLLERR) || (epoll_event_list[i].events & EPOLLHUP) || !(epoll_event_list[i].events & EPOLLIN))
				{
					close_sock(epoll_event_list[i].data.fd);
				}
				else if (epoll_event_list[i].data.fd == serverSock)
				{
					pool.enqueue(add_sock);
				}
				else
				{
					errno_cheek("s1");
					pool.enqueue(&CwebServ::handle, this, connection[epoll_event_list[i].data.fd].addrClient, (int)epoll_event_list[i].data.fd);
				}
			}
		}
	}

private:
	sockaddr_in server;
	epoll_event epoll_event_list[QUANTITY_epoll];
	string SERVER_ADDRESS;
	int PORT;
	bool set_port_sddr = 0;
	void handle(sockaddr_in addrClient, int sockConn)
	{
		errno_cheek("h1");
		handle_handle(addrClient, sockConn);
	}
};
