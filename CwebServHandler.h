//这个全不用管
#include <iostream>
#include "CwebServResources.h"
#include "CwebServErrno.h"
#include "CwebServFunction.h"
#include "cws_file.h"
#include "cws_string.h"
#include <stdio.h>
#include <string.h>
#pragma once
#define int long long
using namespace std;

void close_sock(int sockConn)
{
	// cout << "end" << endl;
	if(connection[sockConn].socket.socket == true)
	{
		if(connection[sockConn].socket.websocket == true)
		{
			;
		}
		else
		{
			connection[sockConn].socket.app.app_(sockConn, connection[sockConn].headers.headers["route"], "", connection[sockConn].headers.headers, connection[sockConn].headers.cookie, 2);
		}
	}
	connection[sockConn] = empty_struct_connection;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, sockConn, NULL);
	errno_cheek("close sock2");
	close(sockConn);
	errno_cheek("close sock3");
}
void add_sock()
{
	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);
	while (1)
	{
		int sockConn = accept(serverSock, (sockaddr *)&addrClient, &len);
		if (sockConn == -1)
		{
			if (errno != 11)
			{
				errno_cheek();
			}
			else
			{
				errno = 0;
			}
			break;
		}
		// cout<<"add_sock "<<sockConn<<endl;
		connection[sockConn].start = true;
		connection[sockConn].addrClient = addrClient;
		epoll_event epoll_event_conn;
		epoll_event_conn.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
		epoll_event_conn.data.fd = sockConn;
		epoll_ctl(epollfd, EPOLL_CTL_ADD, sockConn, &epoll_event_conn);
		if (errno_cheek() != 0)
		{
			close_sock(sockConn);
		}
	}
}
void connection_clear(int sockConn)
{
	sockaddr_in addrClient = connection[sockConn].addrClient;
	connection[sockConn] = empty_struct_connection;
	connection[sockConn].addrClient = addrClient;
	// connection[sockConn].type = "";
	// connection[sockConn].state = 0;
	// connection[sockConn].headers.headers = empty_map;
	// connection[sockConn].headers.cookie = empty_map;
	// connection[sockConn].headers.length = 0;
	// connection[sockConn].headers.sum = "";
	// connection[sockConn].headers.first = true;
	// connection[sockConn].body.type = "none";
	// connection[sockConn].body.length = 0;
	// connection[sockConn].body.remaining_length = 0;
	// connection[sockConn].body.sum = "";
	// connection[sockConn].body.big = false;
	// connection[sockConn].body.file_name = "";
	// connection[sockConn].body.datas = empty_map;
	// connection[sockConn].body.sum_name = "";
	// connection[sockConn].body.sum_content = "";
	// connection[sockConn].body.content = false;
	// connection[sockConn].body.multipart_form_data_content.begin = 1;
	// connection[sockConn].body.multipart_form_data_content.content.clear();
	// connection[sockConn].body.multipart_form_data_content.headers.clear();
}
map<string, string> handle_cookie(string string_Buf)
{
	char Buf[100000] = "";
	strcpy(Buf, string_Buf.c_str());
	map<string, string> cookie;
	cookie.erase(cookie.begin(), cookie.end());
	int length = strlen(Buf);
	string sum_name = "";
	string sum_content = "";
	bool content = false;
	for (int i = 0; i < length; i++)
	{
		if (Buf[i] == '=')
		{
			content = true;
		}
		else if (Buf[i] == ';')
		{
			cookie[sum_name] = sum_content;
			sum_name = "";
			sum_content = "";
			content = false;
			i++;
		}
		else
		{
			if (content)
			{
				sum_content += Buf[i];
			}
			else
			{
				sum_name += Buf[i];
			}
		}
	}
	cookie[sum_name] = sum_content;
	return cookie;
}
int handle_headers(int sockConn, string recvBuf)
{
	string Buf = connection[sockConn].content + recvBuf;
	connection[sockConn].content = "";
	long long length = Buf.length();
	for (int i = 0; i < length; i++)
	{
		if (Buf[i] == '\n')
		{
			if (connection[sockConn].headers.sum == "")
			{
				connection[sockConn].content = Buf.substr(i + 1);
				if (connection[sockConn].headers.headers.count("method") == 0 || (connection[sockConn].headers.headers["method"] != "GET" && connection[sockConn].headers.headers["method"] != "POST"))
				{
					return -1;
				}
				if (connection[sockConn].headers.headers.count("route") == 0)
				{
					return -1;
				}
				if (connection[sockConn].headers.headers.count("Cookie") != 0)
				{
					connection[sockConn].headers.cookie = handle_cookie(connection[sockConn].headers.headers["Cookie"]);
				}
				connection[sockConn].type = connection[sockConn].headers.headers["method"];
				if (connection[sockConn].type == "GET")
				{
					connection[sockConn].state = 2;
				}
				else if (connection[sockConn].type == "POST")
				{
					if (connection[sockConn].headers.headers.count("Content-Length") != 0)
					{
						string str_num = connection[sockConn].headers.headers["Content-Length"];
						for(int i=0; i<str_num.length(); i++)
						{
							connection[sockConn].body.length=connection[sockConn].body.length*10+str_num[i]-'0';
						}
						connection[sockConn].body.remaining_length = connection[sockConn].body.length;
						if (connection[sockConn].body.length > 100000)
						{
							connection[sockConn].body.big = true;
						}
						connection[sockConn].state = 1;
					}
					else
					{
						connection[sockConn].body.length = connection[sockConn].body.remaining_length = 0;
						connection[sockConn].state = 2;
					}
					if (connection[sockConn].headers.headers.count("Content-Type") != 0)
					{
						connection[sockConn].body.type = connection[sockConn].headers.headers["Content-Type"].substr(0, connection[sockConn].headers.headers["Content-Type"].find(";"));
						if (connection[sockConn].body.type == "multipart/form-data")
						{
							connection[sockConn].body.multipart_form_data_content.separator = connection[sockConn].headers.headers["Content-Type"].substr(connection[sockConn].headers.headers["Content-Type"].find("boundary=") + 9);
							kmp_pre(sockConn, connection[sockConn].body.multipart_form_data_content.separator);
						}
					}
				}
				return i;
			}
			if (connection[sockConn].headers.first)
			{
				connection[sockConn].headers.first = false;
				connection[sockConn].headers.headers["method"] = connection[sockConn].headers.sum.substr(0, connection[sockConn].headers.sum.find(" "));
				connection[sockConn].headers.headers["route"] = connection[sockConn].headers.sum.substr(connection[sockConn].headers.sum.find(" ") + 1, connection[sockConn].headers.sum.find(" ", connection[sockConn].headers.sum.find(" ") + 1) - connection[sockConn].headers.sum.find(" ") - 1);

				// cout << connection[sockConn].headers.headers["route"] << endl;
			}
			else
			{
				connection[sockConn].headers.headers[connection[sockConn].headers.sum.substr(0, connection[sockConn].headers.sum.find(":"))] = connection[sockConn].headers.sum.substr(connection[sockConn].headers.sum.find(":") + 2);
			}
			connection[sockConn].headers.sum = "";
		}
		else if (Buf[i] != '\r')
		{
			connection[sockConn].headers.sum += Buf[i];
			connection[sockConn].headers.length++;
		}
		if (connection[sockConn].headers.length > 50000)
		{
			return -1;
		}
	}
	return 1;
}
int handle_body_headers(int sockConn, string Buf)
{
	long long length = Buf.length();
	if (connection[sockConn].body.multipart_form_data_content.begin)
	{
		// connection[sockConn].body.multipart_form_data_content.content.push_back("");
		connection[sockConn].body.multipart_form_data_content.headers.clear();
		connection[sockConn].body.multipart_form_data_content.begin=false;
	}
	for (int i = 0; i < length; i++)
	{
		if (Buf[i] == '\n')
		{
			if (connection[sockConn].body.sum == "")
			{
				connection[sockConn].body.multipart_form_data_content.begin=true;
				return i;
			}
			string header_data=connection[sockConn].body.sum.substr(connection[sockConn].body.sum.find(":") + 1);
			for(int i=0;i<header_data.length();i++)
			{
				if(header_data[i]!=' ')
				{
					header_data = header_data.substr(i);
					break;
				}
			}
			connection[sockConn].body.multipart_form_data_content.headers[connection[sockConn].body.sum.substr(0, connection[sockConn].body.sum.find(":"))] = header_data;
			connection[sockConn].body.sum = "";
		}
		else if (Buf[i] != '\r')
		{
			connection[sockConn].body.sum += Buf[i];
			connection[sockConn].body.length++;
		}
		// if (connection[sockConn].body.length > 50000)
		// {
		// 	return -2;
		// }
	}
	return -1;
}
int handle_datas(int sockConn, string recvBuf)
{
	CWS_string Buf = connection[sockConn].content + recvBuf;
	connection[sockConn].content = "";
	long long length = Buf.length();
	int sum_length = min(connection[sockConn].body.remaining_length, length);
	string body_type = connection[sockConn].body.type;
	// cout << 333 << endl;
	if (body_type == "application/x-www-form-urlencoded")
	{
		if (connection[sockConn].body.big)
		{
			return -1;
		}
		for (int i = 0; i < sum_length; i++)
		{
			if (Buf[i] == '=')
			{
				connection[sockConn].body.content = true;
			}
			else if (Buf[i] == '&')
			{
				connection[sockConn].body.datas[connection[sockConn].body.sum_name] = connection[sockConn].body.sum_content;
				connection[sockConn].body.sum_name = "";
				connection[sockConn].body.sum_content = "";
				connection[sockConn].body.content = false;
			}
			else
			{
				if (connection[sockConn].body.content)
				{
					connection[sockConn].body.sum_content += Buf[i];
				}
				else
				{
					connection[sockConn].body.sum_name += Buf[i];
				}
				if (connection[sockConn].body.remaining_length - i - 1 == 0)
				{
					connection[sockConn].body.datas[connection[sockConn].body.sum_name] = connection[sockConn].body.sum_content;
					connection[sockConn].body.sum_name = "";
					connection[sockConn].body.sum_content = "";
					connection[sockConn].body.content = false;
					break;
				}
			}
		}
	}
	else if (body_type == "multipart/form-data")
	{
		// cout << "multrpart" << endl;
		// Buf.println();
		long long kmp_result = 0;
		long long headers_result = 0;
		long long sep_length = connection[sockConn].body.multipart_form_data_content.separator.length();
		if (connection[sockConn].body.multipart_form_data_content.part == 0)
		{
			kmp_result = kmp(sockConn, Buf);
			// cout << "mm " << kmp_result << endl;
			if (kmp_result != -1)
			{
				connection[sockConn].body.multipart_form_data_content.part = 1;
			}
			kmp_result+=3;
		}
		string ss=Buf.to_string();
		for(long long pos=kmp_result;pos<sum_length;pos++)
		{
			if (connection[sockConn].body.multipart_form_data_content.part == 1)
			{
				headers_result = handle_body_headers(sockConn, Buf.to_string(pos, sum_length));
				if(headers_result!=-1)
				{
					if(connection[sockConn].body.multipart_form_data_content.headers.count("Content-Disposition")!=0)
					{
						string hc=connection[sockConn].body.multipart_form_data_content.headers["Content-Disposition"];
						int name_pos=hc.find("name=");
						name_pos=hc.find("\"",name_pos+5)+1;
						int name_end_pos=hc.find("\"", name_pos);
						string name=hc.substr(name_pos, name_end_pos-name_pos);
						connection[sockConn].body.datas[name]="";
						connection[sockConn].body.headers[name]=connection[sockConn].body.multipart_form_data_content.headers;
						connection[sockConn].body.multipart_form_data_content.content=&connection[sockConn].body.datas[name];
					}
					else
					{
						connection[sockConn].body.datas["empty"]="";
						connection[sockConn].body.headers["empty"]=connection[sockConn].body.multipart_form_data_content.headers;
						connection[sockConn].body.multipart_form_data_content.content=&connection[sockConn].body.datas["empty"];
					}
					connection[sockConn].body.multipart_form_data_content.part=2;
					pos+=headers_result+1;
				}
				else
				{
					pos=sum_length;
				}
			}
			if (connection[sockConn].body.multipart_form_data_content.part == 2)
			{
				// cout<<"*********************"<<endl;
				// Buf.substr(pos,sum_length-pos).println();
				// cout<<"*********************"<<endl;
				// kmp_result = kmp(sockConn, Buf.substr(pos,sum_length-pos));
				// cout<<Buf.length()<<" "<<sum_length-pos<<endl;
				// cout << "mm " << kmp_result << endl;
				kmp_result = Buf.to_string(pos,sum_length-pos).find(connection[sockConn].body.multipart_form_data_content.separator);
				// string xx=Buf.to_string(pos,sum_length-pos);
				// kmp_result=xx.find(connection[sockConn].body.multipart_form_data_content.separator);
				if(kmp_result==string::npos) kmp_result=-1;
				if (kmp_result != -1)
				{
					// cout<<"sep "<<sep_length<<endl;
					connection[sockConn].body.multipart_form_data_content.content->operator+=(Buf.substr(pos,kmp_result-4));
					pos+=kmp_result+sep_length;
					connection[sockConn].body.multipart_form_data_content.part = 1;
					if(pos+1>=sum_length) break;
					if(Buf[pos]=='-' && Buf[pos+1]=='-') break;
					pos+=1;
				}
				else
				{
					connection[sockConn].body.multipart_form_data_content.content->operator+=(Buf.substr(pos,sum_length-pos));
					pos=sum_length;
				}
			}
		}
	}
	else if (body_type == "application/json")
	{
		;
	}
	if (connection[sockConn].body.big)
	{
		connection[sockConn].body.sum = "";
	}
	else
	{
		connection[sockConn].body.sum += Buf.to_string(0, sum_length);
	}
	connection[sockConn].body.remaining_length -= sum_length;
	if (connection[sockConn].body.remaining_length == 0)
	{
		connection[sockConn].content = Buf.to_string(sum_length);
		connection[sockConn].state = 2;
	}
	return 1;
}
std::pair<web_app *, int> handle_route(int sockConn)
{
	string link = connection[sockConn].headers.headers["route"];
	bool flag = 0;
	if (connection[sockConn].headers.headers["route"].size() > 1 && connection[sockConn].headers.headers["route"][connection[sockConn].headers.headers["route"].size() - 1] == '/')
	{
		unsigned int len = connection[sockConn].headers.headers["route"].size();
		while (len > 1)
		{
			if (connection[sockConn].headers.headers["route"][len - 1] == '/')
				len--;
			else
				break;
		}
		ConnList[std::this_thread::get_id()] = sockConn;
		return_redirect(connection[sockConn].headers.headers["route"].substr(0, len));
		return make_pair(nullptr, 302);
	}
	else
	{
		string input_route = connection[sockConn].headers.headers["route"];
		return Route.search(input_route);
	}
}
void handle_handle(sockaddr_in addrClient, int sockConn) //, unique_lock con_lock)
{
	errno_cheek("hh-3");
	// unique_lock<mutex> con_lock2(connection_lock2[sockConn], defer_lock);
	errno_cheek("hh-2");

	int recv_len = 0;
	char recvBuf[10000];
	errno_cheek("hh-1");
	while (1)
	{
		// errno_cheek("hh0");
		// errno_cheek("hh1");
		recv_len = recv(sockConn, recvBuf, sizeof(recvBuf), MSG_DONTWAIT);
		// continue;
		// cout << recvBuf;
		if(errno!=11)
		{
			errno_cheek();
		}
		else
		{
			errno=0;
		}
		string buf(recvBuf, max(recv_len,(long long)0));
		if (-1 == recv_len)
		{
			// cout<<"ooo"<<endl;
			break;
		}
		else if (!recv_len)
		{
			errno = 0;
			close_sock(sockConn);
			return;
		}
		if (recv_len <= 0)
		{
			if (recv_len == 0)
			{
				errno = 0;
				close_sock(sockConn);
			}
			return;
		}
		if(connection[sockConn].socket.socket == true)
		{
			connection[sockConn].state = 3;
			if(connection[sockConn].socket.websocket)
			{
				;
			}
			else
			{
				connection[sockConn].socket.app.app_(sockConn, connection[sockConn].headers.headers["route"], buf, connection[sockConn].headers.headers, connection[sockConn].headers.cookie, 1);
			}
		}
		else
		{
			connection[sockConn].com_buf+=buf;
		}
		if (connection[sockConn].state == 0)
		{
			long long pos = handle_headers(sockConn, buf);
			if (pos == -1)
			{
				close_sock(sockConn);
				return;
			}
			if (connection[sockConn].state == 1)
			{
				buf = "";
			}
			if (connection[sockConn].state != 0)
			{
				connection[sockConn].app = handle_route(sockConn);
				if(connection[sockConn].app.second==302)
				{
					close_sock(sockConn);
					return;
				}
				if (connection[sockConn].app.second == 404 || connection[sockConn].app.second == -1)
				{
					ConnList[std::this_thread::get_id()] = sockConn;
					return_template("404.html", cws_empty_map, "", "404 Not Found");
					close_sock(sockConn);
					return;
				}
			}
		}
		if (connection[sockConn].state == 1)
		{
			// cout << buf.length() << "^&^& " << connection[sockConn].body.length << endl;
			if (handle_datas(sockConn, buf) == -1)
			{
				close_sock(sockConn);
				return;
			}
		}
		if (connection[sockConn].state == 2)
		{
			ConnList[std::this_thread::get_id()] = sockConn;
            connection[sockConn].app.first->app_(sockConn, connection[sockConn].headers.headers["route"], (body_content){connection[sockConn].body.big, connection[sockConn].body.file_name, connection[sockConn].body.type, connection[sockConn].body.sum, &connection[sockConn].body.datas, &connection[sockConn].body.headers}, connection[sockConn].type, connection[sockConn].headers.headers, connection[sockConn].headers.cookie);
			// sss.lock();
			if(connection[sockConn].socket.socket == false)
			{
				if (connection[sockConn].headers.headers.count("Connection") != 0)
				{
					if (connection[sockConn].headers.headers["Connection"] != "keep-alive")
					{
						// sss.unlock();
						close_sock(sockConn);
						return;
					}
				}
				else
				{
					// sss.unlock();
					close_sock(sockConn);
					return;
				}
				connection_clear(sockConn);
			}
			if(connection[sockConn].socket.socket == true && connection[sockConn].content.length()>0)
			{
				connection[sockConn].state = 3;
				if(connection[sockConn].socket.websocket)
				{
					;
				}
				else
				{
					connection[sockConn].socket.app.app_(sockConn, connection[sockConn].headers.headers["route"], connection[sockConn].content, connection[sockConn].headers.headers, connection[sockConn].headers.cookie, 1);
				}
			}
		}
		errno_cheek("hh3");
	}
	errno_cheek("hh5");
	// con_lock2.lock();
	// con_lock2.unlock();
	errno_cheek("hh6");
	epoll_event epoll_event_conn;
	epoll_event_conn.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
	epoll_event_conn.data.fd = sockConn;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, sockConn, &epoll_event_conn);
}
