#include "View.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Placeable.hpp"
#include "Menu.hpp"

View::View(sf::RenderWindow *_window, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset) :
	window(_window),
	l_offset(_l_offset), t_offset(_t_offset), r_offset(_r_offset), b_offset(_b_offset),
	blocks_w(_blocks_w), blocks_h(_blocks_h)
{
	// Set up for event response
	responderMap.resize(blocks_w * blocks_h);
	privileged_event_responder = NULL;
	
	// Set edges from window size and offsets
	int win_w = window->GetWidth(), win_h = window->GetHeight();
	l_pos = (l_offset >= 0 ? l_offset : win_w  + l_offset);
	t_pos = (t_offset >= 0 ? t_offset : win_h + t_offset);
	r_pos = (r_offset >= 0 ? win_w - r_offset : -r_offset);
	b_pos = (b_offset >= 0 ? win_h - b_offset : -b_offset);
}

View::~View() {
	std::cout << "view destruct" << std::endl;
}

void View::addResponder(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		intcoord c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.x >= blocks_w || c.y < 0 || c.y >= blocks_h) continue;
		responderMap[c.y * blocks_w + c.x].remove(obj);
		responderMap[c.y * blocks_w + c.x].push_back(obj);
	}
}
void View::removeResponder(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		intcoord c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.x >= blocks_w || c.y < 0 || c.y >= blocks_h) continue;
		responderMap[c.y * blocks_w + c.x].remove(obj);
	}
}
bool View::requestPrivilegedEventResponderStatus(EventResponder *resp) {
	if (privileged_event_responder != NULL) return false;
	privileged_event_responder = resp;
	return true;
}
void View::relinquishPrivilegedEventResponderStatus(EventResponder *resp) {
	if (privileged_event_responder == resp)
		privileged_event_responder = NULL;
}

void View::drawRect(sf::Color colour, int block_x, int block_y, int blocks_wide, int blocks_tall, float x_offset, float y_offset) {
	float block_width  = (r_pos - l_pos) / blocks_w;
	float block_height = (b_pos - t_pos) / blocks_h;
	int atX = l_pos + (block_x + x_offset) * block_width;
	int atY = t_pos + (block_y + y_offset) * block_height;
	int width = blocks_wide * block_width;
	int height = blocks_tall * block_height;
	if (atX + width >= r_pos)  width = r_pos - atX;
	if (atY + height >= b_pos) height = b_pos - atY;
	window->Draw(
		sf::Shape::Rectangle(atX, atY, width, height, colour)
	);
}

void View::drawMappedObj(MappedObj *obj) {		// Temporary but useful function to draw a top level object
	char c = obj->col();											// in a very generic way
	sf::Color col;
	if (c == 'l') col = sf::Color::Blue;
	else if (c == 'r') col = sf::Color::Red;
	else if (c == 'w') col = sf::Color::White;
	else col = sf::Color::Black;

	int x = obj->x, y = obj->y;
	intcoord b;
	for (int i=0; i < obj->ground_plan.size(); i++) {
		b = obj->ground_plan[i];
		drawRect(col, b.x + x, b.y + y, 1, 1, obj->a, obj->b);
	}
}

void View::draw() {
	// Everybody loves a checkerboard pattern
	for (int i = 0; i < blocks_w; i++)
		for (int j = 0; j < blocks_h; j++)
			drawRect(
				i%(2)
					? (j%(2) ? sf::Color::Black : sf::Color::White)
					: (j%(2) ? sf::Color::White : sf::Color::Black),
				i, j, 1, 1
			);
}

void View::_dispatchMouseEvent(Event *ev) {
	ev->convertCoords((r_pos - l_pos)/blocks_w, (b_pos - t_pos)/blocks_h);
	if (ev->x < 0 || ev->x >= blocks_w || ev->y < 0 || ev->y >= blocks_h)
		return;

	if (privileged_event_responder != NULL) {
		privileged_event_responder->receiveEvent(ev);
		return;
	}

	dispatchMouseEvent(ev);
}
void View::dispatchMouseEvent(Event *ev) {	
	// Get unit whose centre is closest to mouse. Search this & adjacent blocks.
	std::list<MappedObj *> *objs;
	std::list<MappedObj *>::iterator it;
	const std::type_info *unit_type = &typeid(Unit);
	//const std::type_info *vehicle_type = &typeid(Vehicle);
	float xm = ev->x + ev->a, ym = ev->y + ev->b;
	float xdist, ydist, dist, closest_unit_dist = INFINITAH;
	MappedObj *closest_unit = NULL, *u;
	
	for (int j = ym-1; j <= ym+1; j++) for (int i = xm-1; i <= xm+1; i++) {
		if (i < 0 || i >= blocks_w || j < 0 || j >= blocks_h)
			continue;
		objs = &responderMap[j * blocks_w + i];
		for (it = objs->begin(); it != objs->end(); it++)
			if (typeid(*(u = *it)) == *unit_type) {
				xdist = xm - (u->x + u->a + 0.5), ydist = ym - (u->y + u->b + 0.5);		// add 0.5 to (x,y) to get centre of unit
				xdist *= xdist, ydist *= ydist;
				if (xdist > 0.25 || ydist > 0.25) continue;			// check |xdist| & |ydist| < 0.5
				dist = xdist*xdist + ydist*ydist;
				if (dist < closest_unit_dist)
					closest_unit = u, closest_unit_dist = dist;
			}
	}
	
	if (closest_unit != NULL) 			closest_unit->receiveEvent(ev);
	//else if (closest_vehicle != NULL)	closest_vehicle->receiveEvent(ev);
	
	// Otherwise send to placeable, and otherwise, building, and otherwise, whatever else is there
	MappedObj *placeable = NULL, *building = NULL, *button = NULL;
	const std::type_info *placeable_type = &typeid(Placeable), *building_type = &typeid(Building), *button_type = &typeid(Button);
	objs = &responderMap[ev->y * blocks_w + ev->x];
	for (it = objs->begin(); it != objs->end(); it++)
		if (typeid(*(*it)) == *placeable_type)
			placeable = *it;
		else if (typeid(*(*it)) == *building_type)
			building = *it;
		else if (typeid(*(*it)) == *button_type)
			button = *it;
	if (placeable != NULL)		placeable->receiveEvent(ev);
	else if (building != NULL)	building->receiveEvent(ev);
	else if (button != NULL)	button->receiveEvent(ev);
}


ScrollingView::ScrollingView(sf::RenderWindow *_window, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset) :
	View(_window, _blocks_w, _blocks_h, _l_offset, _t_offset, _r_offset, _b_offset)
{
	scroll_x = scroll_y = 0;
	block_size_x = block_size_y = 20;
}
ScrollingView::~ScrollingView() {
	
}

void ScrollingView::_dispatchMouseEvent(Event *ev) {
	if (ev->x < 0 || ev->y < 0 || ev->x >= r_pos - l_pos || ev->y >= b_pos - t_pos)
		return;
	
	int scrollMargin = 30;
	if (ev->x <= scrollMargin)					scroll_x -= scrollMargin - ev->x;
	else if (ev->x >= r_pos - scrollMargin)		scroll_x += ev->x - r_pos + scrollMargin;
	if (ev->y <= scrollMargin)					scroll_y -= scrollMargin - ev->y;
	else if (ev->y >= b_pos - scrollMargin)		scroll_y += ev->y - b_pos + scrollMargin;

	int max_scroll_x = block_size_x * blocks_w - (r_pos - l_pos);
	int max_scroll_y = block_size_y * blocks_h - (b_pos - t_pos);
	if (scroll_x < 0) scroll_x = 0;
	else if (scroll_x >= max_scroll_x) scroll_x = max_scroll_x;
	if (scroll_y < 0) scroll_y = 0;
	else if (scroll_y >= max_scroll_y) scroll_y = max_scroll_y;
	
	ev->convertCoords(block_size_x, block_size_y, scroll_x, scroll_y);
	
	if (privileged_event_responder != NULL)
		privileged_event_responder->receiveEvent(ev);
	else dispatchMouseEvent(ev);
}
void ScrollingView::drawRect(sf::Color colour, int block_x, int block_y, int blocks_wide, int blocks_tall, float a_offset, float b_offset) {
	int atX = (a_offset + block_x) * block_size_x - scroll_x;
	int atY = (b_offset + block_y) * block_size_y - scroll_y;
	int width = blocks_wide * block_size_x;
	int height = blocks_tall * block_size_y;
	if (atX >= r_pos || atY >= b_pos) return;
	if (atX + width >= r_pos)  width = r_pos - atX;
	if (atY + height >= b_pos) height = b_pos - atY;
	window->Draw(
		sf::Shape::Rectangle(atX, atY, width, height, colour)
	);
}
