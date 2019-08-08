#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <climits>
using namespace std;


struct Node{
	unsigned key;
	//vector<pair<unsigned,unsigned> >* Nodes_weight;
	int P; 
	int D;
};

struct Edge{
	unsigned A;
	unsigned B;
	int weight;
};

struct cmp_D{
	bool operator()(Node* a, Node* b){
		if(a->D == b->D)
			return a->key < b->key;
		else 
			return a->D < b->D;
	}
};

struct cmp_Edge{
	bool operator()(Edge* a, Edge* b){
		return a->weight <= b->weight;
	}
};

typedef set<Node*, cmp_D> Nodes;
typedef set<Edge*, cmp_Edge> Edges;


bool topological_sort(unsigned * in_degrees, Edges* E, unsigned N){
	queue<unsigned> sources;
	for(unsigned i=0; i<N; i++){
		if(in_degrees[i]==0)
			sources.push(i);
	}
	unsigned count = 0;
	while(!sources.empty()){
		for(auto i = E[sources.front()].begin(); i != E[sources.front()].end(); i++){
			if((--in_degrees[(*i)->B]) == 0)
				sources.push((*i)->B);
		}
		sources.pop();
		count++;
	}
	return (count == N);
}

pair<bool,int> MST(const vector<Node*> & nodes, Edges* E_origin, Edges * E_dest){
	nodes[0]->D = 0;
	bool exist = true;
	int TotalWeight = 0;
	Nodes T_prime(nodes.begin(), nodes.end());
	while(!T_prime.empty()){
		auto it = T_prime.begin();
		unsigned a = (*it)->key, b;
		//cout<<a<<endl;
		int w;
		TotalWeight += (*it)->D;
		if((*it)->D < INT_MAX)
			T_prime.erase(it);
		else{
			exist = false;
			break;
		}
		auto i1 = E_origin[a].begin();
		while(i1 != E_origin[a].end()){
			b = (*i1)->B;
			w = (*i1)->weight;
			if(w < nodes[b]->D){
				auto it_update = T_prime.find(nodes[b]);
				if (it_update != T_prime.end()){
					T_prime.erase(it_update);
					nodes[b]->D = w;
					nodes[b]->P = a;
					T_prime.insert(nodes[b]);
				}
			}
			++i1;
		} 
		auto i2 = E_dest[a].begin();
		while(i2 != E_dest[a].end()){
			b = (*i2)->A;
			w = (*i2)->weight;
			if(w < nodes[b]->D){
				auto it_update = T_prime.find(nodes[b]);
				if (it_update != T_prime.end()){
					T_prime.erase(it_update);
					nodes[b]->D = w;
					nodes[b]->P = a;
					T_prime.insert(nodes[b]);
				}
			}
			++i2;
		}
	}
	return make_pair(exist,TotalWeight);
}

int main(){
	std::ios::sync_with_stdio(false);
	std::cin.tie(0);
	unsigned N;
	cin>>N;
	vector<Node*> nodes;
	Edges E_origin[N];
	Edges E_dest[N];
	for(unsigned i = 0; i < N; i++){
		E_origin[i].clear();
		E_dest[i].clear();
	}
	unsigned in_degrees[N] = {0};
	for(unsigned i=0; i<N; i++){
		Node* n = new Node;
		//vector<pair<unsigned,unsigned> >* n_w = new vector<pair<unsigned,unsigned> >;
		*n = {i, -1, INT_MAX};
		nodes.push_back(n);
	}	
	unsigned start, end;
	int weight;
	while(!cin.eof()){
		if(!(cin>>start>>end>>weight)) break;
		Edge * e = new Edge({start,end,weight});
		E_origin[start].insert(e);
		E_dest[end].insert(e);
		in_degrees[end]++;
	}	
	if(topological_sort(in_degrees, E_origin, N))
		cout<<"The graph is a DAG"<<endl;
	else
		cout<<"The graph is not a DAG"<<endl;
	pair<bool,int> ans = MST(nodes, E_origin, E_dest);
	if(ans.first)
		cout<<"The total weight of MST is "<<ans.second<<endl;
	else
		cout<<"No MST exists!"<<endl;
	
	while(!nodes.empty()){
		delete nodes.back();
		nodes.pop_back();
	}	
	for(unsigned i = 0; i < N; i++){
		for(auto j = E_origin[i].begin(); j != E_origin[i].end(); j++){
				delete (*j);
		}
	}
}


