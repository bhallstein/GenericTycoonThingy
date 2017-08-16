/*
 * BinaryHeap.hpp – a dynamic, sorted binary heap
 *
 * BinaryHeap is a binary heap for storing pointers to objects.
 *
 * It is bog standard, but has capabilities not provided by std’s heap functions:
 *
 *     1. It supports live updating of elements by index in O(logN) time with `update_at(index, new_value)`.
 *     2. By maintaining a lookup table, BinaryHeap enables you to update any object in the heap directly,
 *        without knowing its index.
 *        For instance, to update an object *x, you can simply call `update(x, 12)`.
 *        And this is virtually as fast as updating by index.
 *
 * ~~ Crafting Your Objects ~~
 *
 * DEFINITION: the “comparand” of an object is the property by which it will be ordered in the heap.
 *     i.e. if A’s comparand is greater than B’s comparand, A will be ‘above’ B in the heap.
 *     For instance, a MapLoc’s comparand is a float called min_dist.
 *
 * To be used with BinaryHeap, your objects must:
 *     1. Implement the function void setComparand(new_value)
 *           This should overwrite the object’s comparand.
 *           e.g. for MapLoc: `void setComparand(float new_dist) { min_dist = new_dist; }
 *     2. Implement the operator `<`
 *           Your function should take a pointer p to another object of the same class, and compare its 
 *           own comparand to p’s, e.g. `bool operator< (MapLoc *p) { return min_dist < p->min_dist; }`
 * 
 * ~~ Creating Your BinaryHeap Instance ~~
 * 
 * BinaryHeap is a templated class, adapting to the type of your object, and of your object’s comparand.
 * So, if your objects are MapLocs, and MapLoc’s comparand min_dist is a float:
 *     `BinaryHeap<MapLoc*, float> myheap(32);`
 * The parameter is the maximum size of your heap. Attempting to push elements beyond this limit will fail.
 *
 * ~~ Initializing Your Heap ~~
 * 
 * To set up your initial heap, you can call `push` to add elements.
 *   - If you need to feed it a large initial dataset, you may use fast_push instead, which prevents objects from
 *     being sorted as they are added.
 *   - Since this disorders the heap, though, you MUST then call `reheapify`, which is O(n).
 *   - WARNING: BinaryHeap assumes some things about your elements:
 *         - that they are all continguous in memory (i.e. they are held in a vector or contiguously allocated)
 *         - that first element pushed or fast_pushed after instantiating or resetting() IS THE FIRST in said contiguous area of memory
 *         - obviously enough, that your elements do not move in memory after you have added pointers to them
 */

#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <iostream>
#include <queue>
#include <algorithm>

#include "types.h"


inline int log_base2(unsigned int x) {
	int ind = 0;
	while (x >>= 1) ind++;
	return ind;
}
inline int twotothepowerof(unsigned int x) {
	return 1 << x;
}


template <class nodetype, typename comparandtype>
class BinaryHeap {
public:
	typedef std::vector<nodetype> queue_type;
	
	// Methods
	BinaryHeap(int max_size);
	~BinaryHeap();
	void reset();				// Makes the heap effectively as-new.
								// Reusing heaps is better than creating new ones repeatedly, since a BinaryHeap must
								// allocate memory at construction time.

	void fast_push(nodetype x);	// Push onto the heap without sorting into place. Use to initialize, then call...
	void reheapify();			// Set the heap back in order.

	void push(nodetype);
	nodetype pop();
	int size();

	void update(nodetype x, comparandtype new_val);		// Update a stored object to a new comparand value
	void update_at(int i, comparandtype new_val);		// Update at a known location in the underlying array
	//void print();

protected:
	
	// Methods
	void swap_nodes(int ind1, int ind2);
	void up_heap(int _ind);
	void down_heap(int _ind);
	void sort_heap();
	
	// Properties
	int n;
	int length;
	queue_type heap;			// The heap
	int *indices_in_heap;		// indices_in_heap[pointer to x] = index in heap of x
	nodetype x_start;		  	// Pointer to first X
};


/* Implementation (a templated class’s implementation must be in the same file as its declaration.) */

template <class nodetype, typename comparandtype>
BinaryHeap<nodetype, comparandtype>::BinaryHeap(int _n) : n(_n) {
	heap.reserve(n);
	indices_in_heap = (int*) malloc(sizeof(int) * n);	// Allocate node-lookup array
	reset();
}

template <class nodetype, typename comparandtype>
BinaryHeap<nodetype, comparandtype>::~BinaryHeap() {
	free(indices_in_heap);
}

template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::reset() {
	heap.clear();
	length = 0;
}

template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::fast_push(nodetype x) {
	if (length >= n) {
		std::cout << "Binary heap error: fast_push called, but heap is full!" << std::endl;
		return;
	}
	int i = length++;
	if (i == 0) x_start = x;
	heap[i] = x;
	indices_in_heap[x - x_start] = i;
}

template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::swap_nodes(int ind1, int ind2) {
	nodetype x1 = heap[ind1], x2 = heap[ind2];
	// Update at ind1
	heap[ind1] = x2;
	indices_in_heap[x2 - x_start] = ind1;
	// Update at ind2
	heap[ind2] = x1;
	indices_in_heap[x1 - x_start] = ind2;
}
template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::up_heap(int _ind) {
	int ind = _ind, parent_ind = (ind-1)/2;
	
	while (ind > 0 && *heap[parent_ind] < heap[ind]) {
		swap_nodes(parent_ind, ind);
		ind = parent_ind;
		parent_ind = (ind-1)/2;
	}
}
template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::down_heap(int _ind) {
	int ind = _ind, l_child_ind = ind*2+1, r_child_ind = ind*2+2, larger_child_ind;
	nodetype our_node = heap[ind];
	
	while (l_child_ind < length) {
		if (r_child_ind < length)
			larger_child_ind = (*heap[l_child_ind] < heap[r_child_ind] ? r_child_ind : l_child_ind);
		else
			larger_child_ind = l_child_ind;
		if (*our_node < heap[larger_child_ind]) {
			swap_nodes(ind, larger_child_ind);
			ind = larger_child_ind;
			l_child_ind = ind*2+1, r_child_ind = ind*2+2;
		}
		else
			break;
	}
}

template <class nodetype, typename comparandtype>
nodetype BinaryHeap<nodetype, comparandtype>::pop() {
	nodetype popsicle = heap[0];
	swap_nodes(0, --length);
	down_heap(0);
	return popsicle;
}
template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::push(nodetype x) {
	if (length >= n) {
		std::cout << "Binary heap error: push called, but heap is full!" << std::endl;
		return;
	}
	if (length == 0) x_start = x;
	heap[length] = x;
	indices_in_heap[x - x_start] = length++;	
	up_heap(length - 1);
}

/* * * * Interlude – some useful heap-related formulae * * * *
 *                                                           *
 *  (indices and depth are zero-based)                       *
 *                                                           *
 *  depth given a number of nodes n:    log(n)               *
 *  depth at a given index x:           log(x+1)             *
 *  index of first element at row d:    2^(d) - 1            *
 *  index of last element at row d:     2^(d+1) - 2          *
 *                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::reheapify() {
	// To sort a heap: start at the lowest level, calling down_heap on the root of each subtree,
	// and do this for each level until the top of the tree is reached
	int i, j, depth, greatest_depth = log_base2(length);

	for (depth = greatest_depth - 1; depth >= 0; depth--)
		for (i = twotothepowerof(depth) - 1, j = twotothepowerof(depth + 1) - 2; i <= j; i++)
			if (i < length)
				down_heap(i);
}
template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::update_at(int i, comparandtype new_val) {
	if (i < 0 || i >= length) return;
	heap[i]->setComparand(new_val);
	int parent_i = (i-1)/2;	
	if (i == 0 || *heap[i] < heap[parent_i])
		down_heap(i);
	else
		up_heap(i);
}
template <class nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::update(nodetype x, comparandtype new_val) {
	int i = indices_in_heap[x - x_start];
	update_at(i, new_val);
}
template <class nodetype, typename comparandtype>
int BinaryHeap<nodetype, comparandtype>::size() {
	return length;
}

//template <class nodetype, typename comparandtype>
//void BinaryHeap<nodetype, comparandtype>::print() {
//	int i, j, depth, greatest_depth = log_base2(length);
//	for (depth = greatest_depth; depth >= 0; depth--) {
//		for (int i = twotothepowerof(depth) - 1, j = twotothepowerof(depth + 1) - 2; i <= j; i++)
//			if (i < length)
//				std::cout << heap[i]->c << " ";
//		std::cout << std::endl;
//	}
//	std::cout << std::endl;
//}

#endif
