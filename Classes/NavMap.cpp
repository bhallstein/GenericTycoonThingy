#include "NavMap.hpp"

/****   NavNode implementation   ****/

NavNode::NavNode() {
	passable = false;
}
NavNode::~NavNode() {
	// Destructor
}

void NavNode::addNeighbour(NavNode *neighbour) {
	neighbours.remove(neighbour);	 // This may perhaps not be necessary.
	neighbours.push_back(neighbour);
}
void NavNode::removeNeighbour(NavNode *neighbour) {
	neighbours.remove(neighbour);
}
bool NavNode::operator< (NavNode *m) {
	return min_dist > m->min_dist;		// Heap orders larger items first by default. We want the opposite.
}
void NavNode::setComparator(float new_min_dist) {
	min_dist = new_min_dist;
}


/****   NavMap implementation   ****/

NavMap::NavMap(int _w, int _h) {
	w = _w, h = _h;
	int n = w * h;
	maplocs.resize(n);

	int i, j, x, y;
	NavNode *maploc;
	for (int j=0; j < h; j++) {
		for (int i=0; i < w; i++) {
			maploc = &maplocs[j*w + i];
			maploc->x = i;						// NavNodes should know their coordinates
			maploc->y = j;						//
			
			maploc->passable = true;			// Set NavNodes’ passability (all passable for now)
			for (y = j-1; y <= j+1; y++)		// Setup refs to neighbouring maplocs
				for (x = i-1; x <= i+1; x++)
					if (x == i && y == j) ;							// self is not a neighbour
					else if (x < 0 || x >= w || y < 0 || y >= h) ;	// check boundaries
					else if (!maplocs[y*w + x].passable) ;			// exclude impassable neighbours
					else maploc->addNeighbour(&maplocs[y*w + x]);
		}
	}
}
NavMap::~NavMap() {
	maplocs.clear();
}

void NavMap::makePassable(int atX, int atY) {
	int i, j, n = atY * w + atX;
	NavNode *maploc = &maplocs[n];
	maploc->passable = true;
	// Add maploc back to network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else maplocs[j*w + i].addNeighbour(maploc);
}
void NavMap::makeImpassable(int atX, int atY) {
	int i, j, n = atY * w + atX;
	NavNode *maploc = &maplocs[n];
	maploc->passable = false;
	// Remove maploc from network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else maplocs[j*w + i].removeNeighbour(maploc);
}
void NavMap::addImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0 || x + resp->w >= w || y + resp->h >= h)
		return;
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			makeImpassable(i, j);
}
void NavMap::removeImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0 || x + resp->w >= w || y + resp->h >= h)
		return;
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			makePassable(i, j);
}
bool NavMap::isPassableAt(int atX, int atY) {
	return maplocs[atY*w + atX].passable;
}
bool NavMap::getRoute(int fromX, int fromY, int toX, int toY, std::vector<NavNode*> *route) {
	if (fromX < 0 || fromX >= w || fromY < 0 || fromY >= h || toX < 0 || toX >= w || toY < 0 || toY >= h) {
		cout << "out of bounds" << endl;
		return false;
	}
    
	// Navigate from A to B
	// Note: pathfinding is actually done backwards: from the destination to the start.
	// This is so we don’t have to reverse the route after extracting it, since it arrives in reverse order.
	NavNode *A = &maplocs[w*toY + toX], *B = &maplocs[w*fromY + fromX];
	if (!A->passable || !B->passable) {
		cout << (A->passable ? "B" : "A") << " impassable!" << endl;
		return false;
	}
	
	/* Initialisation */
	int n = w * h, _i = 0;
	MisterHeapy<NavNode*, float> open_nodes(n);
	for (int i=0; i < n; i++) {
		NavNode *maploc = &maplocs[i];
		maploc->min_dist = (maploc == A ? 0 : INFINITAH);	// Set nodes’ min_dist to inifinity
		if (maploc->passable)
			open_nodes.fast_push(maploc), _i++;		// Populate heap vector with passable nodes
	}
	A->min_dist = 0;					// Set start node’s min_dist to 0
	open_nodes.reheapify();				// Re-sort heap
	
	cout << "Initialised: " << _i << " open nodes" << endl;
	
	/* Run */
	NavNode *X, *neighbour;
	float dist_via_X;
	bool route_found = false;
	while (open_nodes.size()) {
		X = open_nodes.pop();		// Pop node with lowest dist off heap
		
		//cout << "got lowest dist node:" << X->x << "," << X->y << " d: " << X->min_dist << endl;
		
		if (X->min_dist == INFINITAH) {
			cout << "lowest dist node unreachable!" << endl;
			return false;		// No route is possible.
		}
		if (X == B) {
			cout << "route found!" << endl;
			route_found = true;
			break;
		}
		
		// Recalc neighbours’ min_dists
		//cout << "recalculate neighbours’ min_dists:" << endl;
		for (std::list<NavNode*>::iterator i = X->neighbours.begin(); i != X->neighbours.end(); i++) {
			neighbour = (*i);
			dist_via_X = X->min_dist + ((neighbour->x == X->x || neighbour->y == X->y) ? 1 : 1.41421356);
			//cout << " " << neighbour->x << "," << neighbour->y << ": " << dist_via_X << " vs " << neighbour->min_dist;
			if (dist_via_X < neighbour->min_dist) {
				neighbour->min_dist = dist_via_X;
				neighbour->route_prev = X;
				open_nodes.reheapify();
			}
			//cout << " (" << neighbour->min_dist << ")" << endl;
		}
	}
	if (!route_found) return false;
	
	/* Get route */
	for (X = B; X != A; X = X->route_prev)
		route->push_back(X);
	route->push_back(A);
	
	return true;
}
