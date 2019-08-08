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

void implement(bool verbose, int choice);

int main(int argc, char **argv){
  ios::sync_with_stdio(false);
  cin.tie(0);
  int c;
  bool verbose = 0;
  int choice = -1;
  while (1)
    {
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
      switch (c)
        {
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
  implement(verbose, choice);
}

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

void implement(bool verbose, int choice){
	if (choice<0 || choice >= 3) return;
	int width, height, start_x, start_y, end_x, end_y;
	cin>>width>>height>>start_x>>start_y>>end_x>>end_y;
	bool **reached = new bool*[height]; 
	point **points = new point*[height]; 
	for(int i = 0; i < height; i++){
		reached[i] = new bool[width];
		points[i] = new point[width];
	}
	for(int i=0;i<height;i++){
		for(int j=0; j<width; j++){
			reached[i][j] = 0;
			points[i][j].y = i;
			points[i][j].x = j;
			cin>>points[i][j].weight; 
		}
	}
	priority_queue<point*, compare_t> *PQ;
	switch(choice){
	case 0:
		PQ = new binary_heap<point*, compare_t>;
		break;
	case 1:
		PQ = new unsorted_heap<point*, compare_t>;
		break;
	case 2:
		PQ = new fib_heap<point*, compare_t>;
		break;
	default:
		break;
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
		for(int i=0; i<4; i++){
			N = C;
			if (i==0 && (C->x + 1) < width)
				N = &points[C->y][C->x+1];
			if (i==1 && (C->y + 1) < height)
				N = &points[C->y+1][C->x];
			if (i==2 && (C->x - 1) >= 0)
				N = &points[C->y][C->x-1];
			if (i==3 && (C->y - 1) >= 0)
				N = &points[C->y-1][C->x];
			if (reached[N->y][N->x]) continue;
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
	delete PQ;
	for(int i = 0; i < height; i++){
		delete[] reached[i];
		delete[] points[i];
	}
	delete[] reached;
	delete[] points;
}
