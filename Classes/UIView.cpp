#include "UIView.hpp"
#include "Button.hpp"
#include "Callback.hpp"
#include "ResponderMap.hpp"

UIView::UIView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm, bool _allowDrag) :
	View(_theW, _aniston), dragloop(false), rm(_rm), allowDrag(_allowDrag)
{
	
}

UIView::~UIView()
{
	std::cout << "uiview destruct" << std::endl;
	// Destroy callbacks
	for (std::vector<Callback*>::iterator it = subscribers.begin(); it != subscribers.end(); it++)
		delete *it;
}

void UIView::processMouseEvent(Event *ev) {
	if (dragloop) {
		if (ev->type == Event::MOUSEMOVE) {
			jenniferAniston.setCorner(x + ev->x - drag_initial.x, y + ev->y - drag_initial.y);
			updatePosition();
		}
		else if (ev->type == Event::LEFTCLICK) {
			std::cout << "stopping dragloop" << std::endl;
			rm->relinquishPrivilegedEventResponderStatus(this);
			dragloop = false;
		}
		return;
	}
	Button *b = NULL;
	// Get last button that lies under the mouse
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *c = buttons[i];
		if (c->x <= ev->x && c->x + c->width > ev->x
			&& c->y <= ev->y && c->y + c->height > ev->y)
			b = c;
	}
	if (b == NULL) {
		if (allowDrag && ev->type == Event::LEFTMOUSEDOWN && rm->requestPrivilegedEventResponderStatus(this)) {
			std::cout << "starting dragloop" << std::endl;
			dragloop = true;
			drag_initial.x = ev->x, drag_initial.y = ev->y;
		}
		return;
	}
	if (ev->type == Event::MOUSEMOVE)
		b->setHover();	// Set button to hovered
	else if (ev->type == Event::LEFTCLICK) {
		// Call callbacks
		for (int i=0, n = subscribers.size(); i < n; i++)
			subscribers[i]->call(b->getEvent());
	}
}

void UIView::subscribeToButtons(Callback *_callback) {
	subscribers.push_back(_callback);
}
