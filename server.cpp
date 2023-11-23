#include <bits/stdc++.h>
#include "cws.h"
#include <mutex>
using namespace std;
set<int> room[1000000];
int code[1000000];
mutex mt[1000000];
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
signed main()
{
	CwebServ new_app("0.0.0.0",12347);
	new_app.add_app("/", cc, 1);
	new_app.start();
	fclose(stdin);
}
