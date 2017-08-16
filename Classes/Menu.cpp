#include "Menu.hpp"
#include "Level.hpp"

Menu::Menu(W::Window *_win) : W::GameState(W::GS_OPAQUE), win(_win)
{
	menuview = new MenuView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		win,
		&eh
	);
	addView(menuview);
	menuview->subscribeToButtons(new W::Callback(&Menu::receiveEvent, this));
	
	// Key subscriptions
	eh.subscribeToKey(W::KeyCode::K_Q, new W::Callback(&Menu::receiveEvent, this));
	eh.subscribeToKey(W::KeyCode::K_ESC, new W::Callback(&Menu::receiveEvent, this));
}
Menu::~Menu()
{
	removeView(menuview);
	delete menuview;
}

void Menu::resume(W::Returny *ret) {
//	delete level;
	if (ret->type == W::ReturnyType::KILLER_RETURNY)
		W::popState(W::KillerReturny);
	else if (ret->type == W::ReturnyType::PAYLOAD_RETURNY) {
		if (ret->payload == "replay") startLevel("level1.lua");
	}
}
void Menu::update() {
	
}

void Menu::receiveEvent(W::Event *ev) {
	if (ev->type == W::EventType::KEYPRESS) {
		if (ev->key == W::KeyCode::K_Q || ev->key == W::KeyCode::K_ESC)
			W::popState(W::EmptyReturny);
	}
	else if (ev->type == W::EventType::BUTTONCLICK) {
		if (*((std::string*) ev->_payload) == "start level 1")
			startLevel("level1.lua");
	}
}

void Menu::startLevel(std::string path) {
	W::log << "Starting level: " << path << std::endl;
	try {
		level = new Level(win, path);
		W::pushState(level);
		W::log << "Level started" << std::endl;
	} catch (W::Exception &ex) {
		std::string msg = "Oh noes! ";
		msg.append(ex.msg);
		std::cout << msg << std::endl;
//		theW->warning(msg.c_str());
	}
}

MenuView::MenuView(W::Positioner *_pos, W::Window *_win, W::EventHandler *_eh) : W::UIView(_pos, _win, _eh)
{
	buttons.push_back(new W::Button(280, 160, 240, 110, "start level 1"));
}
void MenuView::draw() {
	for (std::vector<W::Button*>::iterator it = buttons.begin(); it < buttons.end(); it++) {
		W::Button *b = *it;
		drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
	drawText(186, 340, W::Colour::BLACK, (char *) "Welcome to Generic Tycoon Thingy");
	drawText(308, 380, W::Colour::WHITE, (char *) "Click to begin");
}
