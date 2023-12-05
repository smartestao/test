#include <bits/stdc++.h>
#include "cws.h"
#include <mutex>
using namespace std;
set<int> room[1000000];
int code[1000000];
mutex mt[1000000];
shared_timed_mutex asfl;
string version="000";
void cd(SOCKET_APP)
{
	int cc=code[sockConn];
	if(event==2)
	{
		mt[cc].lock();
		room[cc].erase(sockConn);
		int si=room[cc].size();
		for(auto i=room[cc].begin(); i!= room[cc].end(); i++)
		{
			return_string(*i, "l"+to_string(si));
		}
		mt[cc].unlock();
		return;
	}
	if(message=="s")
	{
		mt[cc].lock();
		for(auto i=room[cc].begin(); i!= room[cc].end(); i++)
		{
			return_string(*i, "s");
		}
		mt[cc].unlock();
	}
}
void cc(WEB_APP)
{
	if(headers.count("code")>0)
	{
		if(headers["code"].length()==6)
		{
			set_socket(1,0,cd);
			int cc=atoi(headers["code"].c_str());
			mt[cc].lock();
			code[sockConn]=cc;
			int si=room[cc].size()+1;
			for(auto i=room[cc].begin(); i!= room[cc].end(); i++)
			{
				return_string(*i, "l"+to_string(si));
			}
			room[cc].insert(sockConn);
			mt[cc].unlock();
			return_string(sockConn, "a"+to_string(si));
			return;
		}
	}
	return_string(sockConn, "b");
}
void get_v(WEB_APP)
{
	if(asfl.try_lock_shared())
	{
		return_template_string(version);
		asfl.unlock_shared();
	}
	return_template_string("updating...");
}
void get_x(WEB_APP)
{
	if(asfl.try_lock_shared())
	{
		return_file("ASS.exe");
		asfl.unlock_shared();
	}
	return_template_string("t");
}
void update(WEB_APP)
{
	return_template("update.html");
}
void rec(WEB_APP)
{
	if(data.datas->count("password")==0||data.datas->count("file")==0||data.datas->count("version")==0)
	{
		return_template_string("error");
		return;
	}
	if(data.datas->operator[]("password")!="lichang0")
	{
		return_template_string("error password");
		return;
	}
	map<CWS_string, CWS_string> hh;
	hh["Connection"]="close";
	return_template_string("已成功接收，稍后将完成更新", hh);
	asfl.lock();
	CWS_file ass_file(2, "ASS.exe", 1);
	ass_file=data.datas->operator[]("file");
	version=data.datas->operator[]("version").to_string();
	asfl.unlock();
}
signed main()
{
	CwebServ new_app("0.0.0.0",12347);
	new_app.add_app("/", cc, 1);
	new_app.add_app("/v", get_v, 1);
	new_app.add_app("/x", get_x, 1);
	new_app.add_app("/update", update, 1);
	new_app.add_app("/receive_file", rec, 1);
	new_app.start();
	fclose(stdin);
}
