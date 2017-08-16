#include "NavMap.hpp"

/****   NavNode implementation   ****/

NavNode::NavNode() {
	passable = true;
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
void NavNode::setComparand(float _min_dist) {
	min_dist = _min_dist;
}


/****   NavMap implementation   ****/

NavMap::NavMap(int _w, int _h) : open_nodes(_w * _h) {
	w = _w, h = _h;
	int n = w * h;
	nodes.resize(n);

	int i, j, x, y;
	NavNode *node;
	for (int j=0; j < h; j++) {
		for (int i=0; i < w; i++) {
			node = &nodes[j*w + i];
			node->x = i;						// NavNodes should know their coordinates
			node->y = j;						//
			
			for (y = j-1; y <= j+1; y++)		// Setup refs to neighbouring nodes
				for (x = i-1; x <= i+1; x++)
					if (x == i && y == j) ;							// self is not a neighbour
					else if (x < 0 || x >= w || y < 0 || y >= h) ;	// check map boundaries
					//else if (!nodes[y*w + x].passable) ;			// exclude impassable neighbours (actually don't: all passable at this point!)
					else node->addNeighbour(&nodes[y*w + x]);
		}
	}
}
NavMap::~NavMap() {
	nodes.clear();
}

void NavMap::makePassable(int atX, int atY) {
	int i, j, n = atY * w + atX;
	NavNode *node = &nodes[n], *m1, *m2;
	std::vector<NavNode*> diagonalia;
	node->passable = true;
	// Add node back to network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else {
				nodes[j*w + i].addNeighbour(node);
				if (i == atX || j == atY) diagonalia.push_back(&nodes[j*w + i]);
			}
			
	// Recreate nodelinks between adjacent diagonals
	for (int a=0; a < diagonalia.size(); a++) {
		m1 = diagonalia[a];
		for (int b=0; b < diagonalia.size(); b++) {
			m2 = diagonalia[b];
			if (m1->x != m2->x || m1->y != m2->y) m1->addNeighbour(m2);
		}
	}
}
void NavMap::makeImpassable(int atX, int atY) {
	int i, j;
	NavNode *node = &nodes[atY*w + atX], *m1, *m2;
	std::vector<NavNode*> diagonalia;

	node->passable = false;
	// Remove node from network
	for (j = atY-1; j <= atY+1; j++)
		for (i = atX-1; i <= atX+1; i++)
			if (i == atX && j == atY) ;
			else if (i < 0 || i >= w || j < 0 || j >= h) ;
			else {
				nodes[j*w + i].removeNeighbour(node);
				if (i == atX || j == atY) diagonalia.push_back(&nodes[j*w + i]);
			}
	// Sever nodelinks between adjacent diagonals
	for (int a=0; a < diagonalia.size(); a++) {
		m1 = diagonalia[a];
		for (int b=0; b < diagonalia.size(); b++) {
			m2 = diagonalia[b];
			if (m1->x != m2->x || m1->y != m2->y) m1->removeNeighbour(m2);
		}
	}
}
NavNode* NavMap::nodeAt(int atX, int atY) {
	return &nodes[atY * w + atX];
}
void NavMap::addImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	intcoord c;
	for (int i=0, n = resp->resp_blocks.size(); i < n; i++) {
		c = resp->resp_blocks[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h)
			continue;
		makeImpassable(c.x, c.y);
	}
}
void NavMap::removeImpassableObject(EventResponder *resp) {
	int x = resp->x, y = resp->y;
	intcoord c;
	for (int i=0, n = resp->resp_blocks.size(); i < n; i++) {
		c = resp->resp_blocks[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h)
			continue;
		makePassable(c.x, c.y);
	}
}
bool NavMap::isPassableAt(int atX, int atY) {
	return nodes[atY*w + atX].passable;
}
bool NavMap::getRoute(int fromX, int fromY, int toX, int toY, std::vector<NavNode*> *route) {
	if (fromX < 0 || fromX >= w || fromY < 0 || fromY >= h || toX < 0 || toX >= w || toY < 0 || toY >= h) {
		cout << "out of bounds" << endl;
		return false;
	}
    
	// Navigate from A to B
	// Note: pathfinding is actually done backwards: from the destination to the start.
	// This is so we don’t have to reverse the route after extracting it, since it arrives in reverse order.
	NavNode *A = &nodes[w*toY + toX], *B = &nodes[w*fromY + fromX];
	if (!A->passable || !B->passable) {
		//cout << (A->passable ? "B" : "A") << " impassable!" << endl;
		return false;
	}
	
	/* Initialisation */
	int n = w * h, _i = 0;
	open_nodes.reset();
	for (int i=0; i < n; i++) {
		NavNode *node = &nodes[i];
		node->min_dist = INFINITAH;					// Set nodes’ min_dist to infinity
		if (node->passable)
			open_nodes.fast_push(node), _i++;		// Populate heap vector with passable nodes
	}
	A->min_dist = 0;					// Set start node’s min_dist to 0
	open_nodes.reheapify();				// Re-sort heap
	
	/* Run */
	NavNode *X, *neighbour;
	float dist_via_X;
	bool route_found = false;
	while (open_nodes.size()) {
		X = open_nodes.pop();		// Pop node with lowest dist off heap
		
		if (X->min_dist == INFINITAH) {
			//cout << "lowest dist node unreachable!" << endl;
			return false;		// No route is possible.
		}
		if (X == B) {
			// cout << "route found!" << endl;
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
				open_nodes.update(neighbour, dist_via_X);
				neighbour->route_prev = X;	
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
