/*
 * BinaryHeap.hpp – a dynamic, sorted binary heap
 *
 * BinaryHeap is a subclass of priority_queue, that allows live updating of elements. Updated elements are
 * automatically reordered in the heap in O(logn) time.
 *
 * ~~ Crafting Your Objects ~~
 *
 * DEFINITION: the “comparand” of an object is the property using which you will compare them,
 *     i.e. the criterion by which they’ll be ordered in the heap.
 *     For instance, a MapLoc’s comparand is a float: min_dist.
 *
 * NOTE: BinaryHeap CANNOT hold simple types (ints, etc.), ONLY pointers to your objects.
 *
 * To be used in a BinaryHeap, your objects must:
 *     1. Implement the function void setComparand(new_value)
 *           This should overwrite the property that is used to compare this to other objects.
 *     2. Override the operator `<`
 *           Your function should take a pointer P to another object of the same type, and compare its 
 *           own comparand to that of P, e.g. `return this->min_dist < P->min_dist;`
 * 
 * ~~ Creating Your BinaryHeap Instance ~~
 * 
 * BinaryHeap is a templated class, and needs the type of your object, and of your object’s comparand.
 * e.g. if your object is MapLoc, and MapLoc’s comparand is `float min_dist`:
 *     BinaryHeap<MapLoc*, float> myheap;
 * 
 * ~~ Initializing Your Heap ~~
 * 
 * It is feasible to initialize a heap by `push`ing objects onto it. However, this is O(nlogn).
 *   - If you need to feed it a large initial dataset, you can directly access the heap’s underlying vector,
 *     using the property *vec, then call `reheapify`, which is O(n).
 * Be warned, though: modifying this vector directly will leave the heap all discombobulated.
 *   - You MUST, after modifying vec, call the heap’s `reheapify` function, to resort the entire thing.
 * Also, only use vec/reheapify for large initializations.
 *   - `pop`s, `push`es and `update`s are all cheap: O(logn),
 *
 */

#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <iostream>
#include <queue>
#include <algorithm>


template <typename nodetype>
struct o_less {
	bool operator() (nodetype a, nodetype b) {
		return *a < b;
	};
};

template <typename nodetype, typename comparandtype>
class BinaryHeap : public std::priority_queue<nodetype, std::vector<nodetype>, o_less<nodetype> > {
public:
	using std::priority_queue<nodetype, std::vector<nodetype>, o_less<nodetype> >::c;
	
	// Methods
	BinaryHeap();
	void reheapify();
	void update(int ind, comparandtype new_val);
	void print();
	
	// Properties
	std::vector<nodetype> *vec;

protected:
	
	// Methods
	void up_heap(int _ind);
	void down_heap(int _ind);
	
};


/* Implementation – a templated class’s implementation must be in the same file as its declaration. */

template <typename nodetype, typename comparandtype>
BinaryHeap<nodetype, comparandtype>::BinaryHeap() {
	vec = &c;
}

template <typename nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::reheapify() {
	make_heap(c.begin(), c.end(), o_less<nodetype>());
}

template <typename nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::update(int ind, comparandtype new_val) {
	if (ind < 0 || ind >= c.size()) return;
	c[ind]->setComparand(new_val);
	
	int parent_ind = (ind-1)/2;
	
	if (ind == 0 || *c[ind] < c[parent_ind])
		down_heap(ind);
	else
		up_heap(ind);
}

template <typename nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::up_heap(int _ind) {
	int ind = _ind, parent_ind = (ind-1)/2;
	nodetype our_node = c[ind];
	
	while (ind > 0 && *c[parent_ind] < c[ind]) {
		c[ind] = c[parent_ind];
		c[parent_ind] = our_node;
		ind = parent_ind;
		parent_ind = (ind-1)/2;
	}
}
template <typename nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::down_heap(int _ind) {
	int ind = _ind, len = c.size();
	int l_child_ind = ind*2+1, r_child_ind = ind*2+2, larger_child_ind;
	nodetype our_node = c[ind];
	
	while (l_child_ind < len) {
		if (r_child_ind < len)
			larger_child_ind = (*c[l_child_ind] < c[r_child_ind] ? r_child_ind : l_child_ind);
		else
			larger_child_ind = l_child_ind;
		if (*our_node < c[larger_child_ind]) {
			c[ind] = c[larger_child_ind];
			c[larger_child_ind] = our_node;
			ind = larger_child_ind;
			l_child_ind = ind*2+1, r_child_ind = ind*2+2;
		}
	}
}

template <typename nodetype, typename comparandtype>
void BinaryHeap<nodetype, comparandtype>::print() {
	for (int i=0; i < c.size(); i++)
		std::cout << c[i]->comparand << ' ';
	std::cout << std::endl;
}


#endif
