/*
* @Author: Zhou Yee
* @Date:   2023-03-19 19:21:32
* @Last Modified by:   zy
* @Last Modified time: 2023-03-19 23:38:19
*/
#include "gvl.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
using namespace std;

#define _DEBUG_

#ifdef _DEBUG_
#define DEBUG_PRINT(state) cout<<state.to_string()<<endl;
#else
#define DEBUG_PRINT(state)
#endif


int n;
vector<int> src;
vector<int> goal;

gvl::Graph g("G");

int state_num = 0;

struct State{
	int id;
	vector<int> board;
	int blank;
	int g;
	int h;
	int f;
	State(vector<int> board, int g, int h){
		this->board = board;
		this->g = g;
		this->h = h;
		this->f = g + h;
		for(int i = 0, size = n*n; i < size; ++i){
			if(board[i] == 0){
				blank = i;
				break;
			}
		}
		id = state_num++;
	}
	void update(int g){
		this->g = g;
		this->f = g + h;
	}
	bool operator < (const State& state) const{
		return f < state.f;
	}
	string to_string(){
		stringstream buf;
		buf<<"state"<<id<<"\\n";
		for(int i = 0, size = n*n; i < size; i++){
			if(i%n == 0 && i != 0){
				buf<<"\\n";
			}
			buf<<" "<<board[i];
		}
		buf<<"\\nlevel = "<<g<<"\\nmanhattan = "<<h<<"\\ncost = "<<f;
		return buf.str();
	}
	string id_str(){
		stringstream buf;
		buf<<id;
		return buf.str();
	}
};

/*!
 * calculate the manhattan distance between board1 and board2
 * */
int manhattan(vector<int>& board1, vector<int>& board2){
	int dis = 0;
	int size = n*n;
	int map1[size][2], map2[size][2];
	for(int i = 0; i<size; ++i){
		map1[board1[i]][0] = map2[board2[i]][0] = i / n;
		map1[board1[i]][1] = map2[board2[i]][1] = i % n;
	}
	for(int i = 1; i <size; ++i){
		dis += abs(map1[i][0] - map2[i][0]) + abs(map1[i][1] - map2[i][1]);
	}
	return dis;
}

/*!
 * calculate the hamming distance between board1 and board2
 * */
int hamming(vector<int>& board1, vector<int>& borad2){
	int dis = 0;
	for(int i = 0, size = n*n; i < size; ++i){
		if(board1[i] != borad2[i]){
			++dis;
		}
	}
	return dis;
}

/*!
 * get neighbors of the state
 * */
void neighbor(State & state, vector<State>& neighbors){
	neighbors.clear();
	int blank = state.blank;
	int row = blank / n, col = blank % n;
	if(row > 0){
		vector<int> board(state.board);
		swap(board[blank], board[blank - n]);
		neighbors.emplace_back(board, state.g + 1, manhattan(board, goal));
	}
	if(row < n-1){
		vector<int> board(state.board);
		swap(board[blank], board[blank + n]);
		neighbors.emplace_back(board, state.g + 1, manhattan(board, goal));
	}
	if(col > 0){
		vector<int> board(state.board);
		swap(board[blank], board[blank - 1]);
		neighbors.emplace_back(board, state.g + 1, manhattan(board, goal));
	}
	if(col < n -1){
		vector<int> board(state.board);
		swap(board[blank], board[blank + 1]);
		neighbors.emplace_back(board, state.g + 1, manhattan(board, goal));
	}
}


void print(vector<int>& board){
	for(int i = 0, size = n*n; i < size; i++){
		if(i%n == 0 && i != 0){
			cout<<endl;
		}
		cout<<" "<<board[i];
	}
	cout<<endl;
}

void print(State & state){
	cout<<"g = "<<state.g<<" h = "<<state.h<<" f = "<<state.f<<endl;
	print(state.board);
}

void link_node(State& cur, State& next){
	g.AddNode(next.id_str(), {{"label", next.to_string()}});
	g.AddEdge(cur.id_str(), next.id_str());
}

/*!
 * A star
 * */
int A_star(State& init){
	vector<State> openlist;
	map<vector<int>, int> closelist;
	openlist.push_back(init);
	closelist[init.board] = 1;
	while(! openlist.empty()){
		State cur = openlist.back();
		DEBUG_PRINT(cur);
		if(cur.board == goal){
			return cur.g;
		}
		openlist.pop_back();
		vector<State> neighbors;
		neighbor(cur, neighbors);
		for(auto next : neighbors){
			DEBUG_PRINT(next);
			if(closelist[next.board] == 0){
				openlist.push_back(next);
				closelist[next.board] = 1;
				link_node(cur, next);
			}else{
				vector<State>::iterator it;
				// tmd，把openlist写成neighbor了，调试好久才找到
				for(it = openlist.begin(); it != openlist.end(); ++it){
					if(it->board == next.board){
						break;
					}
				}
				if(it != openlist.end() && it->g < next.g){
					it->update(next.g);
					link_node(cur, next);
				}
			}
		}
		sort(openlist.rbegin(), openlist.rend());
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	ofstream out("out.gv");
	g.AddGraphProperty("rankdir", "LR");
	g.AddCommonNodeProperty("shape", "box");
	cin>>n;
	int size = n * n;
	src.assign(size, 0);
	goal.assign(size, 0);
	for(int i = 0; i<size; ++i){
		cin>>src[i];
	}
	for(int i = 0; i<size; ++i){
		cin>>goal[i];
	}
	State init(src, 0, manhattan(src, goal));
	g.AddNode(init.id_str(), {{"label", init.to_string()}});
	cout<<A_star(init);
	g.RenderDot(out);
	system("dot -Tpng -o out.png out.gv");
	return 0;
}

/*
3
1 2 8 0 4 3 7 6 5
1 2 3 8 0 4 7 6 5
*/