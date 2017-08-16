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
	// std::cout << responderList.size() << " -> ";
	responderList.push_back(resp);
	// std::cout << responderList.size() << std::endl;
}
void MapLoc::removeResponder(EventResponder *resp) {
	// Remove an eventresponder from the list thereof
	// std::cout << responderList.size() << " -> ";
	responderList.remove(resp);
	// std::cout << responderList.size() << std::endl;
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
	return min_dist < m->min_dist;
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
	// std::cout << "Adding object to responders at " << atX << "," << atY << "... " << std::endl;
	for (int j = y; j < y + resp->h; j++)
		for (int i = x; i < x + resp->w; i++)
			maplocs[j*w + i].addResponder(resp);
	// std::cout << std::endl << std::endl;
}
void GameMap::removeResponder(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	if (x < 0 || y < 0  || x + resp->w >= w || y + resp->h >= h)
		return;
	// std::cout << "Removing object from responders at " << atX << "," << atY << "... " << std::endl;
	for (int j = y; j < y + resp->h; j++)
		for (int i = x; i < x + resp->w; i++)
			maplocs[j*w + i].removeResponder(resp);
	// std::cout << std::endl;
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
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			this->makeImpassable(i, j);
}
void GameMap::removeImpassableObject(EventResponder *resp) {
	for (int j = resp->y; j < resp->y + resp->h; j++)
		for (int i = resp->x; i < resp->x + resp->w; i++)
			this->makeImpassable(i, j);
}
bool GameMap::isPassableAt(int atX, int atY) {
	return maplocs[atY*w + atX].passable;
}
bool GameMap::getRoute(int fromX, int fromY, int toX, int toY, std::vector<MapLoc*> *route) {
	if (fromX < 0 || fromX >= w || fromY < 0 || fromY >= h || toX < 0 || toX >= w || toY < 0 || toY >= h)
		return false;
    
	// Navigate from A to B
	// Note: pathfinding is actually done backwards: from the destination to the start.
	// This is so we don’t have to reverse the route after extracting it, since it arrives in reverse order.
	MapLoc *A = &maplocs[w*toY + toX], *B = &maplocs[w*fromY + fromX];
	if (!A->passable || !B->passable) return false;

	/* Initialisation */
	int n = w * h;
	BinaryHeap<MapLoc*, float> open_nodes;
	std::vector<MapLoc*> *vec = open_nodes.vec;
	vec->reserve(n);				// Reserve mem in advance (faster than expanding dynamically)
	for (int i=0; i < n; i++) {
		MapLoc *maploc = &maplocs[i];
		maploc->min_dist = (maploc == A ? 0 : INFINITAH);	// Set nodes’ min_dist to inifinity
		if (maploc->passable)
			vec->push_back(maploc);		// Populate heap vector with passable nodes
	}
	A->min_dist = 0;					// Set start node’s min_dist to 0
	open_nodes.reheapify();				// Re-sort heap
	
	/* Run */
	MapLoc *X, *neighbour;
	float dist_via_X;
	bool route_found = false;
	while (open_nodes.size()) {
		X = open_nodes.top();			// Pop node with lowest min_dist off
		open_nodes.pop();				// the open node list
		
		if (X->min_dist == INFINITAH) return false;		// No route is possible.
		if (X == B) {
			route_found = true;
			break;
		}
		
		// Recalc neighbours’ min_dists
		for (std::list<MapLoc*>::iterator i = X->neighbours.begin(); i != X->neighbours.end(); i++) {
			neighbour = (*i);
			dist_via_X = X->min_dist + ((neighbour->x == X->x || neighbour->y == X->y) ? 1 : 1.41421356);
			if (dist_via_X < neighbour->min_dist) {
				neighbour->min_dist = dist_via_X;
				neighbour->route_prev = X;
			}
		}
	}
	if (!route_found) return false;
	
	/* Get route */
	for (X = B; X != A; X = X->route_prev)
		route->push_back(X);
	return true;
}
