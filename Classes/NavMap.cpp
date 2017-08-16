#include "NavMap.hpp"
#include "Building.hpp"

/****   NavNode implementation   ****/

NavNode::NavNode() {
	passable = true;
}
NavNode::~NavNode() {
	// Destructor
}

void NavNode::addNeighbour(NavNode *n) {
	neighbours.push_back(n);
}
void NavNode::removeNeighbour(NavNode *n) {
	for (std::vector<NavNode*>::iterator it = neighbours.begin(); it != neighbours.end(); )
		if (*it == n) it = neighbours.erase(it);
		else it++;
}
bool NavNode::hasNeighbour(NavNode *n) {
	for (int i=0; i < neighbours.size(); i++)
		if (neighbours[i] == n)
			return true;
	return false;
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
void NavMap::addImpassableObject(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	intcoord c;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h)
			continue;
		makeImpassable(c.x, c.y);
	}
}
void NavMap::removeImpassableObject(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	intcoord c;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h)
			continue;
		makePassable(c.x, c.y);
	}
}
void NavMap::addBuilding(Building *b) {
	std::vector<NavNode *> edgey_nodes;
	std::vector<intcoord> *plan = &b->ground_plan;
	int bx = b->x, by = b->y;
	
	// for each node X in ground plan
	for (int i=0; i < plan->size(); i++) {
		intcoord c = (*plan)[i];
		c.x += bx, c.y += by;
		if (c.x < 0 || c.y < 0 || c.x >= w || c.y >= h) continue;
		NavNode *X = nodeAt(c.x, c.y);
		
		// for each of neighbour of X, Y
		std::vector<NavNode *> *neighbours = &X->neighbours;
		for (std::vector<NavNode *>::iterator it=neighbours->begin(); it != neighbours->end(); ) {
			NavNode *Y = *it;
			bool Y_is_part_of_building = false;
			for (int j=0; j < plan->size(); j++) {
				c = (*plan)[j];
				if (nodeAt(c.x + bx, c.y + by) == Y) {
					Y_is_part_of_building = true;
					break;
				}
			}
			if (Y_is_part_of_building)
				it++;
			else {
				// if Y is not part of the building, sever links to X and add both to edgey nodes
				Y->removeNeighbour(X);
				it = neighbours->erase(it);
				edgey_nodes.push_back(X);
				edgey_nodes.push_back(Y);
			}
		}
	}
	
	// for each edgey node
	for (int i=0; i < edgey_nodes.size(); i++) {
		NavNode *X = edgey_nodes[i];
		std::vector<NavNode *> *neighbours = &X->neighbours;
		// if any two nodes on adjacent sides are not neighbours of x, sever the connection between them
		NavNode *n1, *n2;
		if (X->y > 0 && X->x < w - 1) {			// check above and right nodes
			n1 = nodeAt(X->x, X->y - 1);
			n2 = nodeAt(X->x + 1, X->y);
			if (!X->hasNeighbour(n1) && !X->hasNeighbour(n2))
				n1->removeNeighbour(n2), n2->removeNeighbour(n1);
		}
		if (X->x < w - 1 && X->y < h - 1) {		// check right and below nodes
			n1 = nodeAt(X->x + 1, X->y);
			n2 = nodeAt(X->x, X->y + 1);
			if (!X->hasNeighbour(n1) && !X->hasNeighbour(n2))
				n1->removeNeighbour(n2), n2->removeNeighbour(n1);
		}
		if (X->y < h - 1 && X->x > 0) {			// check below and left nodes
			n1 = nodeAt(X->x, X->y + 1);
			n2 = nodeAt(X->x - 1, X->y);
			if (!X->hasNeighbour(n1) && !X->hasNeighbour(n2))
				n1->removeNeighbour(n2), n2->removeNeighbour(n1);
		}
		if (X->x > 0 && X->y > 0) {				// check left and above nodes
			n1 = nodeAt(X->x - 1, X->y);
			n2 = nodeAt(X->x, X->y - 1);
			if (!X->hasNeighbour(n1) && !X->hasNeighbour(n2))
				n1->removeNeighbour(n2), n2->removeNeighbour(n1);
		}
	}
	
	// add door connections
	std::vector<door> *doors = &b->doors;
	door *d;
	for (int i=0; i < doors->size(); i++) {
		d = &(*doors)[i];
		intcoord cY, cX = { d->coord.x + bx, d->coord.y + by };
		switch (d->orientation) {
			case LEFTWARD:  cY.x = cX.x - 1; cY.y = cX.y;     break;
			case RIGHTWARD: cY.x = cX.x + 1; cY.y = cX.y;     break;
			case UPWARD:    cY.x = cX.x;     cY.y = cX.y - 1; break;
			case DOWNWARD:  cY.x = cX.x;     cY.y = cX.y + 1; break;
			default: return;
		}
		if (cX.x < 0 || cX.y < 0 || cX.x >= w || cX.y >= h) return;
		if (cY.x < 0 || cY.y < 0 || cY.x >= w || cY.y >= h) return;
		NavNode *X = nodeAt(cX.x, cX.y), *Y = nodeAt(cY.x, cY.y);
		X->addNeighbour(Y);
		Y->addNeighbour(X);
	}
}
void NavMap::removeBuilding(Building *b) {
	
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
		for (std::vector<NavNode*>::iterator it = X->neighbours.begin(); it != X->neighbours.end(); it++) {
			neighbour = (*it);
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
