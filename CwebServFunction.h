#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <cmath>
#include <fstream>
#include "cws_string.h"
#include "cws_file.h"
#pragma once
using namespace std;

//返回随机CWS_string， lengthhi长度
CWS_string random_CWS_string(int length = 20)
{
	static CWS_string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	CWS_string result;
	srand(time(NULL));
	for (int i = 0; i < length; i++)
		result += (char)charset[rand() % charset.length()];
	return result;
}
//不是给你用的
void kmp_pre(int sockConn, CWS_string kmp_str)
{
	int j = 0;
	connection[sockConn].body.multipart_form_data_content.kmp_p[0] = 0;
	for (int i = 1; i < kmp_str.length(); i++)
	{
		while (j > 0 && kmp_str[j] != kmp_str[i])
			j = connection[sockConn].body.multipart_form_data_content.kmp_p[j-1];
		if (kmp_str[j] == kmp_str[i])
			j++;
		connection[sockConn].body.multipart_form_data_content.kmp_p[i] = j;
	}
}
//这也不是
long long kmp(int sockConn, CWS_string kmp_str, int pos = 0)
{
	long long j = connection[sockConn].body.multipart_form_data_content.kmp_j;
	CWS_string separetor = connection[sockConn].body.multipart_form_data_content.separator;
	long long aa=0;
	string aaa=kmp_str.to_string();
	for (long long i = pos; i < kmp_str.length(); i++)
	{
		if(aaa[i]==separetor[0]) aa++;
		while (j > 0 && separetor[j] != kmp_str[i])
			j = connection[sockConn].body.multipart_form_data_content.kmp_p[j-1];
		if (separetor[j] == kmp_str[i])
			j++;
		if (j == separetor.length())
		{
			connection[sockConn].body.multipart_form_data_content.kmp_j = 0;
			return i;
		}
	}
	return -1;
}
//返回一个头，没有内容
//headers:cws_string类型的map，前面是头的名字，后面是内容
//session还没写
void return_headers(map<CWS_string, CWS_string> headers = cws_empty_map, CWS_string sessionid = "")
{
	int sockConn = ConnList[std::this_thread::get_id()];
	if (sessionid != "")
	{
		if (headers.count("set-cookie") > 0)
			headers["set-cookie"] += "; sessionid= " + sessionid;
		else
			headers["set-cookie"] = "sessionid= " + sessionid;
	}
	CWS_string buf = "";
	map<CWS_string, CWS_string>::iterator it;
	for (it = headers.begin(); it != headers.end(); it++)
	{
		buf = buf + it->first + ": " + it->second + "\r\n";
	}
	buf = buf + "\r\n";
	for (int i = 0; i < buf.length(); i += 5000)
	{
		send(sockConn, buf.to_string(i, 5000).c_str(), min((long long)5000, buf.length() - i), MSG_NOSIGNAL);
	}
}
//直接返回字符串
void return_string(int sockConn, CWS_string message)
{
	for (int i = 0; i < message.length(); i += 5000)
	{
		send(sockConn, message.to_string(i, 5000).c_str(), min((long long)5000, message.length() - i), MSG_NOSIGNAL);
	}
}
//返回html文件，文件里面可以用{{xx}}写，change前面对于xx，后面对应要替换成的内容，status是状态码，我也不知道为啥不能设置头
void return_template(CWS_string file_name, map<CWS_string, CWS_string> change = cws_empty_map, CWS_string sessionid = "", CWS_string status = "200 OK")
{
	int sockConn = ConnList[std::this_thread::get_id()];
	CWS_string handle_buf;
	CWS_file buf(1, file_name, 1);
	long long length = buf.length();
	long long handle_length = 1;
	CWS_string temp;
	char current, last;
	current = buf[0];
	handle_buf.fill_value(0, current);
	for (long long i = 1; i < length; i++)
	{
		last = current;
		current = buf[i];
		if (last == current && current == '{')
		{
			temp = "";
			int j = i + 1;
			char current_j, next;
			current_j = buf[j], next = buf[j + 1];
			while (!(current_j == next && current_j == '}') && j < length)
			{
				temp += (char)current_j;
				j++;
				current_j = next;
				next = buf[j + 1];
			}
			if (change.count(temp) != 0)
			{
				handle_length -= 1;
				CWS_string str = change[temp];
				for (unsigned int k = 0; k < str.length(); k++)
				{
					handle_buf[handle_length] = str[k];
					handle_length++;
				}
			}
			i = j + 1;
		}
		else
		{
			handle_buf[handle_length].fill_value(buf[i]);
			handle_length++;
		}
	}
	long long length_ = handle_buf.length();
	handle_buf = ("HTTP/1.1 " + status + "\r\ncontent-type: text/html;charset=utf-8\r\nContent-Length: " + to_string(length_) + "\r\n" + ((sessionid != "") ? ("set-cookie: sessionid=" + sessionid + "\r\n") : "") + "\r\n").to_string().c_str() + handle_buf;
	for (int i = 0; i < handle_buf.length(); i += 5000)
	{
		send(sockConn, handle_buf.to_string(i, 5000).c_str(), min((long long)5000, handle_buf.length() - i), MSG_NOSIGNAL);
	}
}
//返回文件
void return_file(CWS_string file_name, map<CWS_string, CWS_string> headers = cws_empty_map, CWS_string sessionid = "")
{
	int sockConn = ConnList[std::this_thread::get_id()];
	CWS_file file(1, file_name, 1);
	long long file_length = file.length();
	if (sessionid != "")
	{
		if (headers.count("set-cookie") > 0)
			headers["set-cookie"] += "; sessionid= " + sessionid;
		else
			headers["set-cookie"] = "sessionid= " + sessionid;
	}
	CWS_string buf = "";
	if (headers.count("type") > 0)
	{
		buf = buf + headers["type"] + " ";
		headers.erase("type");
	}
	else
		buf = buf + response_headers["type"] + " ";
	if (headers.count("code") > 0)
	{
		buf = buf + headers["code"] + "\r\n";
		headers.erase("code");
	}
	else
		buf = buf + response_headers["code"] + "\r\n";
	if (headers.count("Content-Type") > 0)
	{
		buf = buf + "Content-Type: " + headers["Content-Type"] + "\r\n";
		headers.erase("Content-Type");
	}
	else
		buf = buf + "Content-Type: " + "application/octet-stream" + "\r\n";
	if (headers.count("Content-Length") > 0)
	{
		buf = buf + "Content-Length: " + headers["Content-Length"] + "\r\n";
		headers.erase("Content-Length");
	}
	else
	{
		// cout<<file_length<<endl;
		buf = buf + "Content-Length: " + to_string(file_length) + "\r\n";
	}
	if (headers.count("Content-Disposition") > 0)
	{
		buf = buf + "Content-Disposition: " + headers["Content-Disposition"] + "\r\n";
		headers.erase("Content-Disposition");
	}
	else
		buf=buf+"Content-Disposition: attachment; filename="+file_name+"\r\n";
	map<CWS_string, CWS_string>::iterator it;
	for (it = headers.begin(); it != headers.end(); it++)
	{
		buf = buf + it->first + ": " + it->second + "\r\n";
	}
	buf = buf + "\r\n";
	send(sockConn, buf.to_string().c_str(), buf.length(), MSG_NOSIGNAL);
	int aa=0;
	for (long long pos = 0; pos < file_length; pos += 5000)
	{
		string content=file.to_std_string(pos,5000);
		aa+=content.length();
		// cout<<content.length()<<endl;
		if(send(sockConn, content.c_str(), content.length(), MSG_NOSIGNAL)==-1)
		{
			return;
		}
		// cout<<pos<<endl;
	}
	// cout<<file_length<<"  "<<aa<<endl;
}
//返回字符串
void return_template_string(CWS_string message, map<CWS_string, CWS_string> headers = cws_empty_map, CWS_string sessionid = "")
{
	// cout<<std::this_thread::get_id()<<"  2"<<endl;
	int sockConn = ConnList[std::this_thread::get_id()];
	if (sessionid != "")
	{
		if (headers.count("set-cookie") > 0)
			headers["set-cookie"] += "; sessionid= " + sessionid;
		else
			headers["set-cookie"] = "sessionid= " + sessionid;
	}
	CWS_string buf = "";
	if (headers.count("type") > 0)
	{
		buf = buf + headers["type"] + " ";
		headers.erase("type");
	}
	else
		buf = buf + response_headers["type"] + " ";
	if (headers.count("code") > 0)
	{
		buf = buf + headers["code"] + "\r\n";
		headers.erase("code");
	}
	else
		buf = buf + response_headers["code"] + "\r\n";
	if (headers.count("Content-Type") > 0)
	{
		buf = buf + "Content-Type: " + headers["Content-Type"] + "\r\n";
		headers.erase("Content-Type");
	}
	else
		buf = buf + "Content-Type: " + response_headers["Content-Type"] + "\r\n";
	if (headers.count("Content-Length") > 0)
	{
		buf = buf + "Content-Length: " + headers["Content-Length"] + "\r\n";
		headers.erase("Content-Length");
	}
	else
		buf = buf + "Content-Length: " + to_string(message.length()) + "\r\n";
	map<CWS_string, CWS_string>::iterator it;
	for (it = headers.begin(); it != headers.end(); it++)
	{
		buf = buf + it->first + ": " + it->second + "\r\n";
	}
	buf = buf + "\r\n";
	buf = buf + message;
	send(sockConn, buf.to_string().c_str(), buf.length(), MSG_NOSIGNAL);
}
//没写完，别用
CWS_string set_session(map<CWS_string, CWS_string> &cookie, CWS_string name, CWS_string value)
{
	int sockConn = ConnList[std::this_thread::get_id()];
	if (cookie.count("sessionid") == 0)
	{
		CWS_string id = random_CWS_string();
		session[id][name] = value;
		return id;
	}
	else
	{
		session[cookie["sessionid"]][name] = value;
		return "";
	}
}
//重定向，target是目标
void return_redirect(CWS_string target, CWS_string sessionid = "")
{
	int sockConn = ConnList[std::this_thread::get_id()];
	CWS_string buf = "HTTP/1.1 302 Found\r\ncontent-type: text/html;charset=utf-8\r\nLocation: " + target + ((sessionid != "") ? ("\r\nset-cookie: sessionid=" + sessionid + "\r\n") : "\r\n") + "\r\n";
	send(sockConn, buf.to_string(0, 40000).c_str(), buf.length(), MSG_NOSIGNAL);
}
//没写完
CWS_string query_session(map<CWS_string, CWS_string> cookie, CWS_string name)
{
	int sockConn = ConnList[std::this_thread::get_id()];
	if (cookie.count("sessionid") != 0 && session[cookie["sessionid"]].count(name) != 0)
	{
		return session[cookie["sessionid"]][name];
	}
	return "none";
}
