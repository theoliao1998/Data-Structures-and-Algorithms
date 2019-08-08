#ifndef FIB_HEAP_H
#define FIB_HEAP_H

#include <algorithm>
#include <cmath>
#include "priority_queue.h"

// OVERVIEW: A specialized version of the 'heap' ADT implemented as a 
//           Fibonacci heap.
template<typename TYPE, typename COMP = std::less<TYPE> >
class fib_heap: public priority_queue<TYPE, COMP> {
public:
  typedef unsigned size_type;

  // EFFECTS: Construct an empty heap with an optional comparison functor.
  //          See test_heap.cpp for more details on functor.
  // MODIFIES: this
  // RUNTIME: O(1)
  fib_heap(COMP comp = COMP());

  // EFFECTS: Deconstruct the heap with no memory leak.
  // MODIFIES: this
  // RUNTIME: O(n)
  ~fib_heap();
  
  // EFFECTS: Add a new element to the heap.
  // MODIFIES: this
  // RUNTIME: O(1)
  virtual void enqueue(const TYPE &val);

  // EFFECTS: Remove and return the smallest element from the heap.
  // REQUIRES: The heap is not empty.
  // MODIFIES: this
  // RUNTIME: Amortized O(log(n))
  virtual TYPE dequeue_min();

  // EFFECTS: Return the smallest element of the heap.
  // REQUIRES: The heap is not empty.
  // RUNTIME: O(1)
  virtual const TYPE &get_min() const;

  // EFFECTS: Get the number of elements in the heap.
  // RUNTIME: O(1)
  virtual size_type size() const;

  // EFFECTS: Return true if the heap is empty.
  // RUNTIME: O(1)
  virtual bool empty() const;
  
private:
  // Note: compare is a functor object
  COMP compare;

private:
  // Add any additional member functions or data you require here.
  // You may want to define a strcut/class to represent nodes in the heap and a
  // pointer to the min node in the heap.
  class FibNode{
  public:	
	TYPE key;
	int degree;
	FibNode * left;
	FibNode * right;
	FibNode * child;
	FibNode * parent;
	FibNode(const TYPE & val = NULL):key(val), degree(0),
            left(this),right(this),child(NULL),parent(NULL){}
  };
  int n;
  int maxdegree();
  FibNode *min;
  void consolidate();
  void addnode(FibNode* x,FibNode* n);
  void fibHeapLink(FibNode* y,FibNode* x);
};

template<typename TYPE, typename COMP>
fib_heap<TYPE, COMP> :: fib_heap(COMP comp) {
    compare = comp;
    n = 0;
    min = NULL; 
}

template<typename TYPE, typename COMP>
fib_heap<TYPE, COMP> :: ~fib_heap() {
    while(n>0){
		dequeue_min();
	}
}

template<typename TYPE, typename COMP>
const TYPE &fib_heap<TYPE, COMP> :: get_min() const {
    return min->key;
}

template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: addnode(FibNode* x,FibNode* y) {
    x->left = y->left;
	y->left->right = x;
	x->right = y;
	y->left = x;
}

template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: enqueue(const TYPE &val) {
    FibNode* x=new FibNode(val);
    if(min==NULL)
		min = x;
	else{
		addnode(x,min);
		if(compare(x->key, min->key))
			min = x;
	}
	n++;
}

template<typename TYPE, typename COMP>
TYPE fib_heap<TYPE, COMP> :: dequeue_min() {
    FibNode* z = min;
    TYPE result = z-> key;
    
    FibNode* x = z->child;
    
    FibNode* y = z->child;
    
    FibNode* node;
    int number = 0;
    if (x!= NULL) {
		number = 1;
		while(x->left != y){
			number++;
			x = x->left;
		}
	}
	x = y;
	
    for(int i=0; i<number; i++){
		node = x->left;
		addnode(x,min);
		x->parent = NULL;
		x = node;
	}
    z->left->right = z->right;
    z->right->left = z->left;
    min = z->left;
    delete z;
    n--;
    if(n==0) min = NULL;
    else consolidate();
	return result;
}

template<typename TYPE, typename COMP>
int fib_heap<TYPE, COMP> :: maxdegree() {
    double phi = (1.0 + sqrt(5.0))/2.0;
    int result = log(n)/log(phi);
    return result;
}


template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: consolidate() {
    FibNode** A = new FibNode*[maxdegree()+1];
    for(int i=0; i<=maxdegree(); i++)
		A[i] = NULL;
    FibNode* x = min;
    if(x==NULL) return;
    int number = 1;
    FibNode* node = x;
    while(x->left != node){
		number++;
		x = x->left;
	}
	x = node;
    for(int i = 0; i<number; i++){
		int d = x->degree;
		while(A[d]!=NULL){
			FibNode* y = A[d];
			if(compare(y->key,x->key)){
				FibNode* temp = y;
				y =x;
				x = temp;
			}
			fibHeapLink(y,x);
			A[d]=NULL;
			d++;
		}
		A[d] = x;
		x = x->left;
	}
	min = NULL;
	for(int i=0; i<=maxdegree();i++){
		if(A[i]!=NULL){
			if(min==NULL)
				min = A[i];
			else{
				if(compare(A[i]->key,min->key))
					min = A[i];
			}
		}
	}
	delete[] A;
}

template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: fibHeapLink(FibNode* y, FibNode* x) {
    y->left->right = y->right;
	y->right->left = y->left;
	if(x->child==NULL){
		x->child = y;
		y->left = y;
		y->right = y;
	}
	else{
		addnode(y,x->child);
	}
	y->parent = x;
	x->degree++;
}

template<typename TYPE, typename COMP>
unsigned fib_heap<TYPE, COMP> :: size() const {
    return n;
}

template<typename TYPE, typename COMP>
bool fib_heap<TYPE, COMP> :: empty() const {
    return (min == NULL);
}
#endif //FIB_HEAP_H
