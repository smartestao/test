#include <bits/stdc++.h>
#include "cws.h"
using namespace std;
double x_min,x_max,y_min,y_max,zoom;
int n,m;
struct point
{
	double x;
	double y;
	int id;
}point[100000];
int h[100000],ne[100000],e[100000],idx;
double d[100000];
map<string,string> information;
void add(int a,int b)
{
	e[idx]=b;
	ne[idx]=h[a];
	d[idx]=sqrt((point[a].x-point[b].x)*(point[a].x-point[b].x)+(point[a].y-point[b].y)*(point[a].y-point[b].y));
	h[a]=idx++;
}
void load()
{
	ifstream infile;
   	infile.open("map.in");
	memset(h,-1,sizeof(h));
	idx=0;
	infile>>x_min>>x_max>>y_min>>y_max>>zoom;
	infile>>n>>m;
	for(int i=1;i<=n;i++)
	{
		int id;
		double x,y;
		infile>>id>>x>>y;
		point[id].id=id;
		point[id].x=x;
		point[id].y=y;
	}
	for(int i=1;i<=m;i++)
	{
		int a,b;
		infile>>a>>b;
		add(a,b);
	}
	infile.close();
	CWS_file infile2(1, "inf.csv", 1);
	string inf;
	long long pos=0, pos2=0;
	while(1)
	{
		pos2=infile2.find(";;;", pos);
		if(pos2==-1) break;
		inf=infile2.to_std_string(pos,pos2-pos);
		information[inf.substr(0, inf.find(','))]=inf.substr(inf.find(',')+1);
		pos=pos2+4;
	}
}
double calc(int a,int b)
{
	return sqrt((point[a].x-point[b].x)*(point[a].x-point[b].x)+(point[a].y-point[b].y)*(point[a].y-point[b].y));
}
string astar(int start,int target)
{
	if(start<0||start>=100000) return "none";
	if(target<0||target>=100000) return "none";
    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> heap;
    double dis[100000];
    int ls[100000];
    while(!heap.empty()) heap.pop();
    for(int i=0;i<100000;i++) dis[i]=100000;
	memset(ls,-1,sizeof(ls));
    heap.push({calc(start,target), start});
    dis[start]=0;
    while(heap.size())
    {
        pair<double,int> t = heap.top();
        heap.pop();
        int state = t.second;
        if(state == target) break;
        for(int i=h[state];i!=-1;i=ne[i])
        {
        	int j=e[i];
        	if(dis[j] > dis[state] + d[i])
            {
                dis[j] = dis[state] + d[i];
                ls[j] = state;
                heap.push({calc(j,target) + dis[j], j});
            }
		}
    }
    string res = "{\"type\": \"FeatureCollection\",\"features\": [{\"type\": \"Feature\",\"properties\": {\"stroke\": \"#ff0000\", \"stroke-width\": 3.5},\"geometry\": {\"type\": \"LineString\",\"coordinates\": [";
    int pos = target;
    while(pos != start)
    {
		if(pos<0||pos>=100000) return "none";
        res += "["+to_string(point[pos].x)+","+to_string(point[pos].y)+"],";
        pos = ls[pos];
        if(pos==-1) return "none";
    }
    res += "["+to_string(point[start].x)+","+to_string(point[start].y)+"]";
    res += "]}}]}";
    return res;
}
void exit_1(WEB_APP){
	exit(666);
}
void query(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_template_string(astar(atoi(data.datas["starting_point"].c_str()),atoi(data.datas["target_point"].c_str())),headerss);
}
void map_g(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("G.geojson",headerss);
}
void map_1(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("1.geojson",headerss);
}
void map_2(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("2.geojson",headerss);
}
void map_3(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("3.geojson",headerss);
}
void map_4(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("4.geojson",headerss);
}
void map_5(WEB_APP){
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	return_file("5.geojson",headerss);
}
void text(WEB_APP)
{
	map<CWS_string,CWS_string> headerss=cws_empty_map;
	headerss["Access-Control-Allow-Origin"]="*";
	headerss["Content-Type"]="application/json";
	if(information.count(tail.substr(6))>0)
	{
		return_template_string(information[tail.substr(6)], headerss);
	}
	else
	{
		return_template_string("# 没这个", headerss);
	}
}
signed main()
{
	load();
	CwebServ new_app("0.0.0.0",12345);
	new_app.add_app("/exit",exit_1,1);
	new_app.add_app("/query",query,1);
	new_app.add_app("/G.json",map_g,1);
	new_app.add_app("/1.json",map_1,1);
	new_app.add_app("/2.json",map_2,1);
	new_app.add_app("/3.json",map_3,1);
	new_app.add_app("/4.json",map_4,1);
	new_app.add_app("/5.json",map_5,1);
	new_app.add_app("/text",text,2,1);
	new_app.start();
	fclose(stdin);
}
