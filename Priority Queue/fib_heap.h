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
  //A class to represent nodes in the heap
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
  FibNode *min;
  //EFFECT: calculate the allowed max degree D(n)
  int maxdegree();
  void consolidate();
  //EFFECT: add node x to be n's sibling, that is, n's parent's child
  void addnode(FibNode* x,FibNode* n);
  //EFFECT: add node x to be y's child
  void fibHeapLink(FibNode* y,FibNode* x);
  //EFFECT: check whether x is in the array A with size given
  bool belong(FibNode* x, FibNode** A, int size);
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
		dequeue_min(); // delete all the nodes in the heap
	}
}

template<typename TYPE, typename COMP>
const TYPE &fib_heap<TYPE, COMP> :: get_min() const {
    return min->key;
}

template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: addnode(FibNode* x,FibNode* y) {
	x->left->right = x->right; //make the siblings has correct right and left
	x->right->left = x->left;
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
    FibNode* x = NULL;
    TYPE result = z-> key;
    //link every child of min to the root list
    while(z->child!=NULL){
		x = z->child;
		if(x->left == x)
			z->child = NULL;
		else
			z->child = x->left;
		
		addnode(x,min);
		x->parent = NULL;
	}
    
    z->left->right = z->right;
    z->right->left = z->left; 
    if(z->left == z)
		min = NULL;
	else {
		min = z->left;
		
		consolidate();
	}
    delete z;
    n--;
	return result;
}

template<typename TYPE, typename COMP>
int fib_heap<TYPE, COMP> :: maxdegree() {
    double phi = (1.0 + sqrt(5.0))/2.0;
    int result = log(n)/log(phi);
    return result;
}

template<typename TYPE, typename COMP>
bool fib_heap<TYPE, COMP> :: belong(FibNode* x, FibNode** A, int size){
	for(int i=0; i<size; i++)
		if(x==A[i]) return 1;
	
	return 0;
}

template<typename TYPE, typename COMP>
void fib_heap<TYPE, COMP> :: consolidate() {
	
    FibNode** A = new FibNode*[maxdegree()+1];
    for(int i=0; i<=maxdegree(); i++)
		A[i] = NULL;
    FibNode* x = min;
    if(x==NULL) return;
	do{
		int d = x->degree;
		while(A[d]!=NULL){
			//if there exists a sub heap of degree d, connect them together to make a subheap of degree d+1
			FibNode* z = A[d];
			if(compare(z->key,x->key)){
				FibNode* temp = z;
				z =x;
				x = temp;
			}
			fibHeapLink(z,x);
			A[d]=NULL;
			d++;
		}
		A[d] = x;
		x = x->left;
	}while(!belong(x, A, maxdegree()+1));//visit every member in the list
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
	if(x->child==NULL){
		y->left->right = y->right;
		y->right->left = y->left;
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
