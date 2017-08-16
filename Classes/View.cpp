#include "View.hpp"
#include "Unit.hpp"
#include "Building.hpp"
#include "Placeable.hpp"
#include "Menu.hpp"

View::View(sf::RenderWindow *_win, int _grid_w, int _grid_h, int _offset_L, int _offset_T, int _offset_R, int _offset_B) :
	window(_win),
	offset_L(_offset_L), offset_T(_offset_T), offset_R(_offset_R), offset_B(_offset_B),
	grid_w(_grid_w), grid_h(_grid_h)
{
	// Set up for event response
	responderMap.resize(grid_w * grid_h);
	privileged_event_responder = NULL;
	
	// Set edges from window size and offsets
	int win_w = window->GetWidth(), win_h = window->GetHeight();
	l_pos = (offset_L >= 0 ? offset_L : win_w + offset_L);
	t_pos = (offset_T >= 0 ? offset_T : win_h + offset_T);
	r_pos = (offset_R >= 0 ? win_w - offset_R : -offset_R);
	b_pos = (offset_B >= 0 ? win_h - offset_B : -offset_B);
}

View::~View() {
	std::cout << "view destruct" << std::endl;
}

void View::addResponder(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		intcoord c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.x >= grid_w || c.y < 0 || c.y >= grid_h) continue;
		responderMap[c.y * grid_w + c.x].remove(obj);
		responderMap[c.y * grid_w + c.x].push_back(obj);
	}
}
void View::removeResponder(MappedObj *obj) {
	int x = obj->x, y = obj->y;
	for (int i=0, n = obj->ground_plan.size(); i < n; i++) {
		intcoord c = obj->ground_plan[i];
		c.x += x, c.y += y;
		if (c.x < 0 || c.x >= grid_w || c.y < 0 || c.y >= grid_h) continue;
		responderMap[c.y * grid_w + c.x].remove(obj);
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

void View::drawRect(sf::Color colour, int _x, int _y, int _w, int _h, float x_offset, float y_offset) {
	// Calculate block dimensions from current size of view
	float block_width  = (r_pos - l_pos) / grid_w;
	float block_height = (b_pos - t_pos) / grid_h;
	// Calculate resulting rect position & dimensions
	int atX = l_pos + (_x + x_offset) * block_width;
	int atY = t_pos + (_y + y_offset) * block_height;
	int width  = _w * block_width;
	int height = _h * block_height;
	// Don't bother drawing things outside the view
	if (atX > r_pos || atY > b_pos || atX + width < l_pos || atY + height < t_pos)
		return;
	if (atX + width > r_pos)  width  = r_pos - atX;
	if (atY + height > b_pos) height = b_pos - atY;
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
	for (int i = 0; i < grid_w; i++)
		for (int j = 0; j < grid_h; j++)
			drawRect(
				i%(2)
					? (j%(2) ? sf::Color::Black : sf::Color::White)
					: (j%(2) ? sf::Color::White : sf::Color::Black),
				i, j, 1, 1
			);
}

void View::convertEventToBlockCoordinates(Event *ev) {
	int x = ev->x, y = ev->y;
	int block_pixel_width  = (r_pos - l_pos) / grid_w;
	int block_pixel_height = (b_pos - t_pos) / grid_h;
	
	ev->x = x/block_pixel_width;
	ev->y = y/block_pixel_height;
	ev->a = (float) (x%block_pixel_width) / (float) block_pixel_width;
	ev->b = (float) (y%block_pixel_height) / (float) block_pixel_height;
}
void View::dispatchMouseEvent(Event *ev) {
	convertEventToBlockCoordinates(ev);
	
	if (ev->x < 0 || ev->x >= grid_w || ev->y < 0 || ev->y >= grid_h)
		return;
	
	if (privileged_event_responder != NULL) {
		privileged_event_responder->receiveEvent(ev);
		return;
	}
	
	// Get unit whose centre is closest to mouse. Search this & adjacent blocks.
	std::list<MappedObj *> *objs;
	std::list<MappedObj *>::iterator it;
	const std::type_info *unit_type = &typeid(Unit);
	//const std::type_info *vehicle_type = &typeid(Vehicle);
	float xm = ev->x + ev->a, ym = ev->y + ev->b;
	float xdist, ydist, dist, closest_unit_dist = INFINITAH;
	MappedObj *closest_unit = NULL, *u;
	
	for (int j = ym-1; j <= ym+1; j++) for (int i = xm-1; i <= xm+1; i++) {
		if (i < 0 || i >= grid_w || j < 0 || j >= grid_h)
			continue;
		objs = &responderMap[j * grid_w + i];
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
	objs = &responderMap[ev->y * grid_w + ev->x];
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
	block_pixel_width = block_pixel_height = 20;
}
ScrollingView::~ScrollingView() {
	
}

void ScrollingView::convertEventToBlockCoordinates(Event *ev) {
	int x = ev->x + scroll_x, y = ev->y + scroll_y;
	
	ev->x = x/block_pixel_width;
	ev->y = y/block_pixel_height;
	ev->a = (float) (x%block_pixel_width)  / (float) block_pixel_width;
	ev->b = (float) (y%block_pixel_height) / (float) block_pixel_height;
	
	// Update scroll params if close to edge
	x -= scroll_x, y -= scroll_y;
	if (x < 0 || y < 0 || x >= r_pos - l_pos || y >= b_pos - t_pos)
		return;

	int scrollMargin = 30;
	if (x <= scrollMargin)					scroll_x -= scrollMargin - x;
	else if (x >= r_pos - scrollMargin)		scroll_x += x - r_pos + scrollMargin;
	if (y <= scrollMargin)					scroll_y -= scrollMargin - y;
	else if (y >= b_pos - scrollMargin)		scroll_y += y - b_pos + scrollMargin;
    
	int max_scroll_x = block_pixel_width  * grid_w - (r_pos - l_pos);
	int max_scroll_y = block_pixel_height * grid_h - (b_pos - t_pos);
	if (scroll_x < 0) scroll_x = 0;
	else if (scroll_x >= max_scroll_x) scroll_x = max_scroll_x;
	if (scroll_y < 0) scroll_y = 0;
	else if (scroll_y >= max_scroll_y) scroll_y = max_scroll_y;
}
void ScrollingView::drawRect(sf::Color colour, int _x, int _y, int _w, int _h, float a_offset, float b_offset) {
	// Calculate position & dimensions of rect
	int atX = (a_offset + _x) * block_pixel_width - scroll_x;
	int atY = (b_offset + _y) * block_pixel_height - scroll_y;
	int width  = _w * block_pixel_width;
	int height = _h * block_pixel_height;
	// Don't bother drawing things outside the view
	if (atX > r_pos || atY > b_pos || atX + width < l_pos || atY + height < t_pos)
		return;
	if (atX + width > r_pos)  width  = r_pos - atX;
	if (atY + height > b_pos) height = b_pos - atY;
	window->Draw(
		sf::Shape::Rectangle(atX, atY, width, height, colour)
	);
}
