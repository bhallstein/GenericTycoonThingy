#include "UIView.hpp"
#include "Button.hpp"
#include "Callback.hpp"

UIView::UIView(W *_theW, JenniferAniston &_aniston) : View(_theW, _aniston)
{
	
}

UIView::~UIView()
{
	std::cout << "uiview destruct" << std::endl;
	
}

void UIView::processMouseEvent(Event *ev) {
	Button *b = NULL;
	// Get last button that lies under the mouse
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *c = buttons[i];
		if (c->x <= ev->x && c->x + c->width > ev->x
			&& c->y <= ev->y && c->y + c->height > ev->y)
			b = c;
	}
	if (b == NULL) return;
	if (ev->type == Event::MOUSEMOVE)
		b->setHover();	// Set button to hovered
	else if (ev->type == Event::LEFTCLICK) {
		// Call any callbacks subcribed to that button's associated event
		std::map<std::string, Callback *>::iterator it = subscriptions.find(b->action);
		if (it != subscriptions.end())
			(*it).second->call();
	}
}

void UIView::subscribe(const char *_payload, Callback *_callback) {
	subscriptions[_payload] = _callback;
}
