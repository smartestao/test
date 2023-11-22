#include <map>
#include <string>
#include "cws_string.h"
#pragma once
//最好把这玩意删了
#define int long long
map<string, string> empty_map;
map<CWS_string, CWS_string> cws_empty_map;
map<string, string> response_headers;
map<CWS_string, map<CWS_string, CWS_string>> session;
map<const std::thread::id, int> ConnList;
int serverSock;
int epollfd;
struct body_content
{
	bool big = false;
	string file_name = "";
	string type = "";
	string raw = "";
	map<string, string> datas;
	vector<map<string, string>> headers;
	vector<CWS_string>* m_datas;
};
struct web_app
{
	function<void(int, string, body_content, string, map<string, string>, map<string, string>)> app_;
};
struct socket_app
{
	function<void(int, string, string, map<string, string>, map<string, string>, int)> app_;
};
struct struct_headers
{
	map<string, string> headers = empty_map;
	map<string, string> cookie = empty_map;
	long long length = 0;
	string sum = "";
	bool first = true;
};
struct struct_body
{
	string type = "none";
	long long length = 0;
	long long remaining_length = 0;
	string sum = "";
	bool big = false;
	string file_name = "";
	bool content = false;
	string sum_name = "";
	string sum_content = "";
	map<string, string> datas;
	struct struct_multipart_form_data
	{
		string separator;
		long long part = 0;
		long long kmp_p[100];
		long long kmp_j = 0;
		vector<map<string,string>> headers;
		vector<CWS_string> content;
		bool begin=true;
	} multipart_form_data_content;
};
struct struct_socket
{
	bool socket = false;
	bool websocket = false;
	socket_app app;
};
struct struct_connection
{
	sockaddr_in addrClient;
	string type = "";
	string content = "";
	int state = 0;
	struct_headers headers;
	struct_body body;
	std::pair<web_app *, int> app;
	bool again = false;
	bool start = false;
	string com_buf = "";
	struct_socket socket;
};
struct_connection empty_struct_connection;
struct_connection connection[150000];
bool connection_again[150000];
struct TrieNode
{
	TrieNode *children[53] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	bool routend = 0;
	int type = 0;
	int num = 0;
	web_app app;
};
class Trie_route
{
	TrieNode *root;

public:
	Trie_route()
	{
		root = new TrieNode();
		root->routend = false;
	};
	int handle_this_layer(string link_path)
	{
		if (link_path[link_path.size() - 1] != '/')
		{
			link_path = link_path + "/";
		}
		map<string, string> handled_path;
		handled_path["raw"] = link_path;
		long long path_layer = 0;
		string layer_detac = link_path;
		while (layer_detac.find("/") != layer_detac.npos)
		{
			layer_detac = layer_detac.substr(1, layer_detac.size());
			string this_layer = layer_detac;
			this_layer = this_layer.substr(0, this_layer.find("/"));
			handled_path[to_string(path_layer)] = this_layer;
			layer_detac = layer_detac.substr(this_layer.size(), layer_detac.size());
			path_layer++;
		}
		return path_layer - 1;
	}
	int char_to_int(char word)
	{
		if (word >= 'a' && word <= 'z')
		{
			return abs((word - 'a') % 50);
		}
		else
		{
			return abs((word - 'A' + 26) % 50);
		}
	}
	void insert(std::string word_, web_app in, int type, int num)
	{
		string word = word_.substr(1);
		TrieNode *current = root;
		int layer_ = handle_this_layer(word);
		string c_s;
		string aaa=word;
		for (int i = 0; i < word.size(); i++)
		{
			if (current->children[char_to_int(word[i])] == nullptr)
			{
				current->children[char_to_int(word[i])] = new TrieNode;
			}
			current = current->children[char_to_int(word[i])];
		}
		current->routend = true;
		current->type = type;
		current->num = num;
		current->app = in;
	}
	auto search(std::string word_)
	{
		string word = word_.substr(1);
		// cout << word << endl;
		TrieNode *current = root;
		int layer_ = handle_this_layer(word);
		layer_--;
		// cout << layer_ << endl;
		if (current->routend)
		{
			if (current->type == 1)
			{
				if (layer_ + 1 == 0)
				{
					return make_pair(&current->app, 1);
				}
			}
			else if (current->type == 2)
			{
				if (((layer_ + 1) == (current->num)))
				{
					return make_pair(&current->app, 2);
				}
			}
			else if (current->type == 3)
			{
				return make_pair(&current->app, 3);
			}
		}
		for (int i = 0; i < word.size(); i++)
		{
			if (current->children[char_to_int(word[i])] == nullptr)
			{
				break;
			}
			if (word[i] == '/')
			{
				layer_--;
			}
			int target = char_to_int(word[i]);
			if(target>=0&&target<=53)
				current = current->children[target];
			else
				return make_pair((new web_app), 404);
			if (current->routend && (word[i + 1] == '/' || i == word.size() - 1))
			{
				if (current->type == 1)
				{
					if (layer_ == 0)
					{
						return make_pair(&current->app, 1);
					}
				}
				else if (current->type == 2)
				{
					if (((layer_) == (current->num)))
					{
						return make_pair(&current->app, 2);
					}
				}
				else if (current->type == 3)
				{
					return make_pair(&current->app, 3);
				}
			}
		}
		return make_pair((new web_app), 404);
	}
} Route;
