#include <bits/stdc++.h>
#include "cws.h"
using namespace std;
void cd(SOCKET_APP)
{
	;
}
void cc(WEB_APP)
{
	set_socket(1,0,cd);
}
signed main()
{
	CwebServ new_app("0.0.0.0",12345);
	new_app.add_app("/", cc, 1);
	new_app.start();
	fclose(stdin);
}
