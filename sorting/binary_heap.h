#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <algorithm>
#include "priority_queue.h"

// OVERVIEW: A specialized version of the 'heap' ADT implemented as a binary
//           heap.
template<typename TYPE, typename COMP = std::less<TYPE> >
class binary_heap: public priority_queue<TYPE, COMP> {
public:
  typedef unsigned size_type;

  // EFFECTS: Construct an empty heap with an optional comparison functor.
  //          See test_heap.cpp for more details on functor.
  // MODIFIES: this
  // RUNTIME: O(1)
  binary_heap(COMP comp = COMP());

  // EFFECTS: Add a new element to the heap.
  // MODIFIES: this
  // RUNTIME: O(log(n))
  virtual void enqueue(const TYPE &val);

  // EFFECTS: Remove and return the smallest element from the heap.
  // REQUIRES: The heap is not empty.
  // MODIFIES: this
  // RUNTIME: O(log(n))
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
  // Note: This vector *must* be used in your heap implementation.
  std::vector<TYPE> data;
  // Note: compare is a functor object
  COMP compare;

private:
  // Add any additional member functions or data you require here.
  void swap(size_type id1, size_type id2);
  void percolateUp(size_type id);
  void percolateDown(size_type id);
};

template<typename TYPE, typename COMP>
void binary_heap<TYPE, COMP> :: swap(size_type id1, size_type id2) {
    TYPE temp = data[id1];
    data[id1] = data[id2];
    data[id2] = temp;    
}

template<typename TYPE, typename COMP>
binary_heap<TYPE, COMP> :: binary_heap(COMP comp) {
    compare = comp;
    // Fill in the remaining lines if you need.
    std::vector<TYPE> v;
    data = v;    
}

template<typename TYPE, typename COMP>
void binary_heap<TYPE, COMP> :: percolateUp(size_type id) {
    while (id>0 && compare(data[id],data[(id-1)/2])){
		swap(id, (id-1)/2);
		id = (id-1)/2;
	}
}

template<typename TYPE, typename COMP>
void binary_heap<TYPE, COMP> :: percolateDown(size_type id) {
    size_type j;
    for(j=2*id+1; j<data.size(); j=2*id+1){
		if(j < (data.size()-1) && compare(data[j+1],data[j])) j++;
		if(compare(data[id],data[j])) break;
		swap(id,j);
		id = j;
	}
}

template<typename TYPE, typename COMP>
void binary_heap<TYPE, COMP> :: enqueue(const TYPE &val) {
    // Fill in the body.
    data.push_back(val);
    percolateUp(data.size()-1);
}

template<typename TYPE, typename COMP>
TYPE binary_heap<TYPE, COMP> :: dequeue_min() {
    // Fill in the body.
    TYPE result = data[0];
    data[0] = data[data.size()-1];
    data.pop_back();
    percolateDown(0);
    return result;
}

template<typename TYPE, typename COMP>
const TYPE &binary_heap<TYPE, COMP> :: get_min() const {
    // Fill in the body.
    return data[0];
}

template<typename TYPE, typename COMP>
bool binary_heap<TYPE, COMP> :: empty() const {
    // Fill in the body.
    return data.size() == 0;
}

template<typename TYPE, typename COMP>
unsigned binary_heap<TYPE, COMP> :: size() const { 
    // Fill in the body.
    return data.size();
}

#endif //BINARY_HEAP_H
