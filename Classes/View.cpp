#include "View.hpp"

View::View(sf::RenderWindow *_window, int _blocks_w, int _blocks_h, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0) :
	window(_window),
	l_offset(_l_offset), t_offset(_t_offset), r_offset(_r_offset), b_offset(_b_offset),
	blocks_w(_blocks_w), blocks_h(_blocks_h)
{
	ready_for_event_response = false;
	
	// Set edges from window size and offsets
	int win_w = window->GetWidth(), win_h = window->GetHeight();
	l_pos = (l_offset >= 0 ? l_offset : win_w  + l_offset);
	t_pos = (t_offset >= 0 ? t_offset : win_h + t_offset);
	r_pos = (r_offset >= 0 ? win_w - r_offset : -r_offset);
	b_pos = (b_offset >= 0 ? win_h - b_offset : -b_offset);
}

View::~View() {
	
}

void View::createEventResponderMap() {
	if (ready_for_event_response) return;
	responderMap.resize(blocks_w * blocks_h);
	privileged_event_responder = NULL;
	ready_for_event_response = true;
}
void View::addResponder(EventResponder *resp) {
	if (!ready_for_event_response) return;		// This is really the sort of place one should throw an exception
	int x = resp->x, y = resp->y, w = resp->w, h = resp->h;
	for (int j = y; j < y + h; j++)
		for (int i = x; i < x + w; i++)
			if (i >= 0 && i < blocks_w && j >= 0 && j < blocks_h) {
				responderMap[j * blocks_w + i].remove(resp);
				responderMap[j * blocks_w + i].push_back(resp);
			}
}
void View::removeResponder(EventResponder *resp) {
	if (!ready_for_event_response) return;
	int x = resp->x, y = resp->y, w = resp->w, h = resp->h;
	for (int j = y; j < y + h; j++)
		for (int i = x; i < x + w; i++)
			if (i >= 0 && i < blocks_w && j >= 0 && j < blocks_h)
				responderMap[j * blocks_w + i].remove(resp);
}
bool View::requestPrivilegedEventResponderStatus(EventResponder *resp) {
	if (!ready_for_event_response) return false;
	if (privileged_event_responder != NULL) return false;
	privileged_event_responder = resp;
	return true;
}
void View::relinquishPrivilegedEventResponderStatus(EventResponder *resp) {
	privileged_event_responder = NULL;
}

void View::drawRect(sf::Color colour, int atX, int atY, int width, int height) {
	if (atX + width >= r_pos)  width = r_pos - atX;
	if (atY + height >= b_pos) height = b_pos - atY;
	window->Draw(
		sf::Shape::Rectangle(atX, atY, width, height, colour)
	);
}

void View::draw() {
	// Everybody loves a checkerboard pattern
	int checkerboard_size = 20, i, j;
	for (i = l_pos; i < r_pos; i += checkerboard_size)
		for (j = t_pos; j < b_pos; j += checkerboard_size)
			drawRect(
				i%(2*checkerboard_size)
					? (j%(2*checkerboard_size) ? sf::Color::Black : sf::Color::White)
					: (j%(2*checkerboard_size) ? sf::Color::White : sf::Color::Black),
				i, j, r_pos - i, b_pos - j
			);
}

void View::_acceptEvent(Event *ev) {
	if (!ready_for_event_response) return;

	ev->convertCoords((r_pos - l_pos)/blocks_w, (b_pos - t_pos)/blocks_h);
	if (ev->x < 0 || ev->x >= blocks_w || ev->y < 0 || ev->y >= blocks_h)
		return;

	if (privileged_event_responder != NULL) {
		privileged_event_responder->receiveEvent(ev);
		return;
	}
	acceptEvent(ev);
}
void View::acceptEvent(Event *ev) {
	
}
