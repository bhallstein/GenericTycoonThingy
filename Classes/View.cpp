#include "View.hpp"

View::View(sf::RenderWindow *_window, int _l_offset = 0, int _t_offset = 0, int _r_offset = 0, int _b_offset = 0) {
	window = _window;
	l_offset = _l_offset;
	t_offset = _t_offset;
	r_offset = _r_offset;
	b_offset = _b_offset;
	
	responder_block_width = responder_block_height = 1;
	setup_for_event_response = false;
	
	// Set edges from window size and offsets
	int w_width = window->GetWidth(), w_height = window->GetHeight();
	l_pos = (l_offset >= 0 ? l_offset : w_width  + l_offset);
	t_pos = (t_offset >= 0 ? t_offset : w_height + t_offset);
	r_pos = (r_offset >= 0 ? w_width - r_offset : -r_offset);
	b_pos = (b_offset >= 0 ? w_height - b_offset : -b_offset);
}

View::~View() {
	
}

void View::createEventResponderMap() {
	if (setup_for_event_response) return;
	responderMap_w = (r_pos - l_pos) / responder_block_width;
	responderMap_h = (b_pos - t_pos) / responder_block_height;
	responderMap.resize(responderMap_w * responderMap_h);
	privileged_event_responder = NULL;
	setup_for_event_response = true;
}
void View::addResponder(EventResponder *resp) {
	int x = resp->x, y = resp->y, w = resp->w, h = resp->h;
	for (int j = y; j < y + h; j++)
		for (int i = x; i < x + w; i++)
			if (i >= 0 && i < responderMap_w && j >= 0 && j < responderMap_h) {
				responderMap[j * responderMap_w + i].remove(resp);
				responderMap[j * responderMap_w + i].push_back(resp);
			}
}
void View::removeResponder(EventResponder *resp) {
	int x = resp->x, y = resp->y, w = resp->w, h = resp->h;
	for (int j = y; j < y + h; j++)
		for (int i = x; i < x + w; i++)
			if (i >= 0 && i < responderMap_w && j >= 0 && j < responderMap_h)
				responderMap[j * responderMap_w + i].remove(resp);
}
bool View::requestPrivilegedEventResponderStatus(EventResponder *resp) {
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
			window->Draw(
				sf::Shape::Rectangle(
					i, j, r_pos - i, b_pos - j,
					i%(2*checkerboard_size)
						? (j%(2*checkerboard_size) ? sf::Color::Black : sf::Color::White)
						: (j%(2*checkerboard_size) ? sf::Color::White : sf::Color::Black)
				)
			);
}

void View::_acceptEvent(Event *ev) {
	ev->convertCoords(responder_block_width, responder_block_height);
	if (ev->x < 0 || ev->x >= responderMap_w || ev->y < 0 || ev->y >= responderMap_h)
		return;
	acceptEvent(ev);
}
void View::acceptEvent(Event *ev) {
	
}
