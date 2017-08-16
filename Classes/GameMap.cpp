#include "GameMap.hpp"

/****   MapLoc implementation   ****/

MapLoc::MapLoc() {
	passable = false;
}
MapLoc::~MapLoc() {
	// Destructor
}

void MapLoc::addResponder(EventResponder *resp) {
	// Add an eventresponder to the list thereof
	 //cout << responderList.size() << " -> ";
	responderList.push_back(resp);
	 //cout << responderList.size() << endl;
}
void MapLoc::removeResponder(EventResponder *resp) {
	// Remove an eventresponder from the list thereof
	 //cout << responderList.size() << " -> ";
	responderList.remove(resp);
	 //cout << responderList.size() << endl;
}
void MapLoc::dispatchEvent(Event *ev, EventResponder **p_e_r) {
	for (std::list<EventResponder*>::iterator i = responderList.begin(); i != responderList.end(); i++)
		(*i)->receiveEvent(ev, p_e_r);
}
void MapLoc::addNeighbour(MapLoc *neighbour) {
	neighbours.remove(neighbour);	 // This may perhaps not be necessary.
	neighbours.push_back(neighbour);
}
void MapLoc::removeNeighbour(MapLoc *neighbour) {
	neighbours.remove(neighbour);
}
bool MapLoc::operator< (MapLoc *m) {
	return min_dist > m->min_dist;		// Heap orders larger items first by def. We want the opposite.
}
void MapLoc::setComparator(float new_min_dist) {
	min_dist = new_min_dist;
}


/****   GameMap implementation   ****/

GameMap::GameMap() {
	// Constructor
}
GameMap::~GameMap() {
	maplocs.clear();
}

void GameMap::setDimensions(int _w, int _h) {
	w = _w, h = _h;
	int n = w * h;
	maplocs.resize(n);

	// Test binary heap ordering
	//BinaryHeap<MapLoc*, float> heap;
	//srand(time(0));
	//for (int a = 0; a < 15; a++) {
	//	maplocs[a].min_dist = rand()%10;
	//	heap.push(&maplocs[a]);
	//}
	//heap.print(); heap.pop(); heap.print();

	int i, j, x, y;
	MapLoc *maploc;
	for (int j=0; j < h; j++) {
		for (int i=0; i < w; i++) {
			maploc = &maplocs[j*w + i];
			maploc->x = i;						// MapLocs should know their coordinates
			maploc->y = j;						//
			
			maploc->passable = true;			// Set MapLocs’ passability (all passable for now)
			for (y = j-1; y <= j+1; y++)		// Setup refs to neighbouring maplocs
				for (x = i-1; x <= i+1; x++)
					if (x == i && y == j) ;							// self is not a neighbour
					else if (x < 0 || x >= w || y < 0 || y >= h) ;	// check boundaries
					else if (!maplocs[y*w + x].passable) ;			// exclude impassable neighbours
					else maploc->addNeighbour(&maplocs[y*w + x]);
		}
	}
}

void GameMap::addResponder(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0  || x + resp->w >= w || y + resp->h >= h)
		return;
	// cout << "Adding object to responders at " << atX << "," << atY << "... " << endl;
	for (int j = y; j < y + resp->h; j++)
		for (int i = x; i < x + resp->w; i++)
			maplocs[j*w + i].addResponder(resp);
	// cout << endl << endl;
}
void GameMap::removeResponder(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0  || x + resp->w >= w || y + resp->h >= h)
		return;
	// cout << "Removing object from responders at " << atX << "," << atY << "... " << endl;
	for (int j = y; j < y + resp->h; j++)
		for (int i = x; i < x + resp->w; i++)
			maplocs[j*w + i].removeResponder(resp);
	// cout << endl;
}
void GameMap::dispatchEvent(Event *ev, EventResponder **p_e_r) {
	maplocs[w*ev->y + ev->x].dispatchEvent(ev, p_e_r);
}

void GameMap::makePassable(int atX, int atY) {
	int i, j, n = atY * w + atX;
	MapLoc *maploc = &maplocs[n];
	maploc->passable = true;
	// Add maploc back to network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else maplocs[j*w + i].addNeighbour(maploc);
}
void GameMap::makeImpassable(int atX, int atY) {
	int i, j, n = atY * w + atX;
	MapLoc *maploc = &maplocs[n];
	maploc->passable = false;
	// Remove maploc from network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else maplocs[j*w + i].removeNeighbour(maploc);
}
void GameMap::addImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0  || x + resp->w >= w || y + resp->h >= h)
		return;
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			this->makeImpassable(i, j);
}
void GameMap::removeImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0  || x + resp->w >= w || y + resp->h >= h)
		return;
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			this->makePassable(i, j);
}
bool GameMap::isPassableAt(int atX, int atY) {
	return maplocs[atY*w + atX].passable;
}
bool GameMap::getRoute(int fromX, int fromY, int toX, int toY, std::vector<MapLoc*> *route) {
	if (fromX < 0 || fromX >= w || fromY < 0 || fromY >= h || toX < 0 || toX >= w || toY < 0 || toY >= h) {
		cout << "out of bounds" << endl;
		return false;
	}
    
	// Navigate from A to B
	// Note: pathfinding is actually done backwards: from the destination to the start.
	// This is so we don’t have to reverse the route after extracting it, since it arrives in reverse order.
	MapLoc *A = &maplocs[w*toY + toX], *B = &maplocs[w*fromY + fromX];
	if (!A->passable || !B->passable) {
		cout << (A->passable ? "B" : "A") << " impassable!" << endl;
		return false;
	}
	
	/* Initialisation */
	int n = w * h, _i = 0;
	BinaryHeap<MapLoc*, float> open_nodes(n);
	for (int i=0; i < n; i++) {
		MapLoc *maploc = &maplocs[i];
		maploc->min_dist = (maploc == A ? 0 : INFINITAH);	// Set nodes’ min_dist to inifinity
		if (maploc->passable)
			open_nodes.fast_push(maploc), _i++;		// Populate heap vector with passable nodes
	}
	A->min_dist = 0;					// Set start node’s min_dist to 0
	open_nodes.reheapify();				// Re-sort heap
	
	cout << "Initialised: " << _i << " open nodes" << endl;
	
	/* Run */
	MapLoc *X, *neighbour;
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
		for (std::list<MapLoc*>::iterator i = X->neighbours.begin(); i != X->neighbours.end(); i++) {
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
