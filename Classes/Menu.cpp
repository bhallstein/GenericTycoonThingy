#include "Menu.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Button.hpp"
#include "Callback.hpp"

Menu::Menu(Game *_game, W *_theW) : GameState(_game, _theW)
{
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	menuview = new MenuView(_theW, aniston, &responderMap);
	responderMap.addResponder(menuview);
	
	menuview->subscribeToButtons(new Callback(&Menu::receiveEvent, this));
	
	// Key subscriptions
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_ESC);
}
Menu::~Menu()
{	
	delete menuview;
}

void Menu::pause() {
	
}
void Menu::resume(Returny *returny) {
	delete level;
	if (returny->type == Returny::killer_returny)
		game->stateFinished(this, Returny(Returny::killer_returny));
}
void Menu::update() {

}
void Menu::draw() {
	menuview->_draw();
}

void Menu::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	menuview->updatePosition();
}

void Menu::receiveEvent(Event *ev) {
	if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)
			game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC)
			game->stateFinished(this, Returny(Returny::empty_returny));
	}
	else if (ev->type == Event::BUTTONCLICK) {
		if (ev->payload == "start level 1")
			startLevel("level1.lua");
	}
}

void Menu::startLevel(std::string path) {
	std::string s = "Starting level: "; s += path; s += "\n";
	theW->log(s.c_str());
	try {
		level = new Level(game, theW, path);
		game->pushState(level);
		W::log("Level started.\n");
	} catch (MsgException &ex) {
		std::string msg = "Oh noes! ";
		msg.append(ex.msg);
		theW->warning(msg.c_str());
	}
}


#include "../W.hpp"

MenuView::MenuView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm) : UIView(_theW, _aniston, _rm, DISALLOW_DRAG)
{
	buttons.push_back(new Button(100, 200, 240, 135, "start level 1"));
}
void MenuView::draw() {
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}
