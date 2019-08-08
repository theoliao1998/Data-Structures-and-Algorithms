#include <getopt.h>
#include <iostream>
#include <string>
#include "priority_queue.h"
#include "binary_heap.h"
#include "unsorted_heap.h"
#include "fib_heap.h"
using namespace std;

const string names[3] = {"BINARY", "UNSORTED", "FIBONACCI"};
const int num = 3; 


struct point{
	int y;
	int x;
	int weight;
	int pathcost;
	point* predecessor = NULL;
};

struct compare_t
{
    bool operator()(point* a, point* b) const
    {
        if(a->pathcost == b->pathcost && a->x == b-> x)
			return a->y < b->y;
		else if(a->pathcost == b->pathcost)
			return a->x < b->x;
		else
			return a->pathcost < b->pathcost;
    }
};

void implement(priority_queue<point*, compare_t> * PQ, bool** reached, point** points, bool verbose,int width, int height);

int main(int argc, char **argv){
	ios::sync_with_stdio(false);
	cin.tie(0);
	int c;
	bool verbose = 0;
	int choice = -1;
	while (1){
		static struct option long_options[] =
		{
			{"verbose",  no_argument, 0, 'v'},
			{"implementation",  required_argument, 0, 'i'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vi:",
                       long_options, &option_index);

		if (c == -1)
			break;
		string s;
		switch (c){
			case 'v':  
				verbose = 1;        
				break;
			case 'i':
				s = optarg;
				for(int i=0; i<num; i++){
					if (names[i]==s)
						choice = i;
				}
				break;
			default:
				break;
		}
    }	
	int width, height;
	cin>>width>>height;
	bool **reached = new bool*[height]; //bool map to check whether reached
	point **points = new point*[height]; 
	for(int i = 0; i < height; i++){
		reached[i] = new bool[width];
		points[i] = new point[width];
	}
	if (choice == 0){
		priority_queue<point*, compare_t> * PQ = new binary_heap<point*, compare_t>;
		implement(PQ, reached, points, verbose, width, height);
		delete PQ; // PQ must be deleted in this scope
	}				
	else if (choice ==1){
		priority_queue<point*, compare_t> * PQ = new unsorted_heap<point*, compare_t>;
		implement(PQ, reached, points, verbose, width, height);
		delete PQ;
	}
	else if (choice == 2){
		priority_queue<point*, compare_t> * PQ = new fib_heap<point*, compare_t>;
		implement(PQ, reached, points, verbose, width, height);
		delete PQ;
	}
	for(int i = 0; i < height; i++){
		delete[] reached[i];
		delete[] points[i];
	}
	delete[] reached;
	delete[] points;
}


void trace_back_path(point* N){
	if (N->predecessor == NULL){
		cout<<"("<<N->x<<", "<<N->y<<")"<<endl;
		return;
	}
	else{
		trace_back_path(N->predecessor);
		cout<<"("<<N->x<<", "<<N->y<<")"<<endl;
	}
}

void implement(priority_queue<point*, compare_t> * PQ, bool** reached, point** points, bool verbose, int width, int height){
	int start_x, start_y, end_x, end_y;
	cin>>start_x>>start_y>>end_x>>end_y;
	for(int i=0;i<height;i++){//initialize the grid
		for(int j=0; j<width; j++){
			reached[i][j] = 0;
			points[i][j].y = i;
			points[i][j].x = j;
			cin>>points[i][j].weight; 
		}
	}
	points[start_y][start_x].pathcost = points[start_y][start_x].weight;
	reached[start_y][start_x] = 1;
	PQ -> enqueue(&points[start_y][start_x]);
	int step = 0;
	while(!PQ->empty()){
		point* C = PQ->dequeue_min();
		if(verbose){
			cout<<"Step "<<step<<endl;
			cout<<"Choose cell ("<<C->x<<", "<<C->y<<") with accumulated length "<<C->pathcost<<"."<<endl;		
		}
		point* N;
		for(int i=0; i<4; i++){//check the four neighbors in order
			N = C;
			if (i==0 && (C->x + 1) < width)
				N = &points[C->y][C->x+1];
			if (i==1 && (C->y + 1) < height)
				N = &points[C->y+1][C->x];
			if (i==2 && (C->x - 1) >= 0)
				N = &points[C->y][C->x-1];
			if (i==3 && (C->y - 1) >= 0)
				N = &points[C->y-1][C->x];
			if (reached[N->y][N->x]) continue; // if already reached then skip it
			N->pathcost = C->pathcost + N->weight;
			reached[N->y][N->x] = 1;
			N->predecessor = C;
			if(N->y == end_y && N->x == end_x){
				if(verbose){
					cout<<"Cell ("<<N->x<<", "<<N->y<<") with accumulated length "<<N->pathcost<<" is the ending point."<<endl;
				}
				cout<<"The shortest path from ("<<start_x<<", "<<start_y<<") to ("<<end_x<<", "<<end_y<<") is "<<N->pathcost<<"."<<endl;
				cout<<"Path:"<<endl;
				trace_back_path(N);
				return;
			}
			else{
				PQ->enqueue(N);
				if(verbose)
					cout<<"Cell ("<<N->x<<", "<<N->y<<") with accumulated length "<<N->pathcost<<" is added into the queue."<<endl;
			}	
		}
		step++;			
	}
}
