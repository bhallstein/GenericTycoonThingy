#include "LevelView.hpp"
#include "Unit.hpp"

/* LevelView-specific event types */
namespace W { namespace EventType {
	T LV_LEFTMOUSEDOWN  = W::Event::registerType();
	T LV_RIGHTMOUSEDOWN = W::Event::registerType();
	T LV_LEFTMOUSEUP    = W::Event::registerType();
	T LV_RIGHTMOUSEUP   = W::Event::registerType();
	T LV_MOUSEMOVE      = W::Event::registerType();
} }


/* LevelView implementation */

LevelView::LevelView() :
	View(new W::Positioner(
		W::Corner::TOP_LEFT,
		W::PosType::FIXED, W::PosType::FIXED,
		W::PosType::PROPORTIONAL, W::PosType::PROPORTIONAL,
		0, 0, 1, 1
	)),
	gridsize(20),
	level_width(-1), level_height(-1)
{
	bgRect = new W::DrawnRect(this, rct.pos, rct.sz, W::Colour::White);
	
	// Screenedge scrolling
	W::Callback scrCb(&LevelView::scrollEvent, this);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_LEFT,   scrCb);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_RIGHT,  scrCb);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_TOP,    scrCb);
	W::Messenger::subscribeToEventType(W::EventType::SCREENEDGE_BOTTOM, scrCb);
}
LevelView::~LevelView()
{
	// bye level view
	W::Messenger::unsubscribeFromEventType(W::EventType::SCREENEDGE_LEFT,   this);
	W::Messenger::unsubscribeFromEventType(W::EventType::SCREENEDGE_RIGHT,  this);
	W::Messenger::unsubscribeFromEventType(W::EventType::SCREENEDGE_TOP,    this);
	W::Messenger::unsubscribeFromEventType(W::EventType::SCREENEDGE_BOTTOM, this);
}

void LevelView::processMouseEvent(W::Event *ev) {
	// Convert event from pixel to grid coordinates
	ev->pos -= _offset;
	ev->pos = convertPixelToGridCoords(ev->pos);
	
	// Resubmit converted event with LEVEL_X type
	using namespace W::EventType;
	switch (ev->type) {
		case LEFTMOUSEDOWN  : ev->type = LV_LEFTMOUSEDOWN;  break;
		case RIGHTMOUSEDOWN : ev->type = LV_RIGHTMOUSEDOWN; break;
		case LEFTMOUSEUP    : ev->type = LV_LEFTMOUSEUP;    break;
		case RIGHTMOUSEUP   : ev->type = LV_RIGHTMOUSEUP;   break;
		case MOUSEMOVE      : ev->type = LV_MOUSEMOVE;      break;
	}
//	if (ev->type == LV_LEFTMOUSEUP) std::cout << ev->pos.xyStr() << "\n";
	W::Messenger::dispatchPositionalEvent(ev);
}
W::EventPropagation::T LevelView::scrollEvent(W::Event *ev) {
	using namespace W::EventType;
	int scrollDist = 10;
	if (ev->type == SCREENEDGE_LEFT)        scroll(-scrollDist, 0);
	else if (ev->type == SCREENEDGE_RIGHT)  scroll(scrollDist, 0);
	else if (ev->type == SCREENEDGE_TOP)    scroll(0, -scrollDist);
	else if (ev->type == SCREENEDGE_BOTTOM) scroll(0, scrollDist);
	return W::EventPropagation::SHOULD_CONTINUE;
}
void LevelView::scroll(int x, int y) {
	_offset.x -= x;
	_offset.y -= y;
		// The offset is added to DrawnObjs’ positions to achieve scrolling.
		// When the offset is zero, we view the top left of the map; it can’t be greater than zero
	if (_offset.x > 0) _offset.x = 0;
	if (_offset.y > 0) _offset.y = 0;
		// The max (i.e. maximally negative) offset is the difference between the pixel size of the map
		// and the view’s actual on-screen size
	int max_offset_x = rct.sz.width - level_width * gridsize;
	int max_offset_y = rct.sz.height - level_height * gridsize;
	if (_offset.x < max_offset_x) _offset.x = max_offset_x;
	if (_offset.y < max_offset_y) _offset.y = max_offset_y;
}

W::position LevelView::convertGridToPixelCoords(const W::position &_p) {
	return W::position(
		(int)((_p.x + _p.a) * gridsize),
		(int)((_p.y + _p.b) * gridsize)
	);
}
W::size LevelView::convertGridToPixelCoords(const W::size &_s) {
	return W::size(
		_s.width * gridsize,
		_s.height * gridsize
	);
}
W::position LevelView::convertPixelToGridCoords(const W::position &_p) {
	return W::position(
		_p.x/gridsize,
		_p.y/gridsize,
		float((_p.x)%gridsize) / float(gridsize),
		float((_p.y)%gridsize) / float(gridsize)
	);
}

void LevelView::updatePosition(const W::size &winsize) {
	bgRect->setRect(rct);
}


/* GTTHelpView implementation */

GTTHelpView::GTTHelpView() : W::UIView("UIViews/GTTHelpView.lua")
{
	t1 = new W::DrawnText(this, W::position(220,10), "Help", W::Colour::White);
	t2 = new W::DrawnText(this, W::position(14,54), "place furniture and staff in your", W::Colour::White);
	t3 = new W::DrawnText(this, W::position(14,74), "buildings. customers will come and", W::Colour::White);
	t4 = new W::DrawnText(this, W::position(14,94), "give you money.", W::Colour::White);
	t5 = new W::DrawnText(this, W::position(14,144), "[time left]", W::Colour::White);
	t6 = new W::DrawnText(this, W::position(14,184), "Get cracking! GO!", W::Colour::White);
	
	r1 = new W::DrawnRect(this, W::position(10,234), W::size(500,1), W::Colour(1, 1, 1, 0.3));
	
	t7 = new W::DrawnText(this, W::position(14,264), "Esc: quit to main menu", W::Colour::White);
	t8 = new W::DrawnText(this, W::position(14,284), "Q: quit completely", W::Colour::White);
	t9 = new W::DrawnText(this, W::position(14,304), "H: help", W::Colour::White);
}
void GTTHelpView::setTimeRemaining(int t) {
	timeRemaining = t;
	setTimeRemainingText();
}
void GTTHelpView::setMonetaryTarget(int m) {
	monetaryTarget = m;
}
void GTTHelpView::setTimeRemainingText() {
	char s[100];
	int time_minutes = timeRemaining/60;
	int time_seconds = timeRemaining%60;
	sprintf(s, "you've got %02d:%02d left to earn %c%d", time_minutes, time_seconds, MR_CURRENCY, monetaryTarget);
	t5->setTxt(s);
}
//	// Draw close button
//	W::Button *b = buttons[0];
//	drawRect(b->pos.x,   b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
//	drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, 45);
//	drawRect(b->pos.x+2, b->pos.y+5, 8, 2, W::Colour::White, -45);

