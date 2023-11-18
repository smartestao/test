#include <bits/stdc++.h>
#include "cws.h"
using namespace std;
void rec(WEB_APP)
{
	cout<<sockConn<<" "<<event<<endl;
	if(event==1)
	{
		return_template_string("aaa");
	}
}
signed main()
{
	CwebServ new_app("0.0.0.0",12345);
	new_app.add_app("/", rec, 1, 0, 1);
	new_app.start();
	fclose(stdin);
}
