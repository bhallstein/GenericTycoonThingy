#include "Menu.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Button.hpp"

Menu::Menu(Game *_game, W *_theW) : GameState(_game, _theW), buttonMap(_theW->width(), _theW->height())
{
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	menuview = new MenuView(_theW, aniston, &buttonMap, &buttons);
	responderMap.addResponder(menuview);
	
	// Add buttons
	startlevel_btn = new Button(this, 100, 200, 240, 135);
	buttonMap.addResponder(startlevel_btn);
	buttons.push_back(startlevel_btn);
	
	// Key subscriptions
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_ESC);
}
Menu::~Menu()
{
	// Remove buttons form view
	Button *b;
	for (int i=0, n = buttons.size(); i < n; i++) {
		b = buttons[i];
		buttonMap.removeResponder(b);
		delete b;
	}
	buttons.clear();
	
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
	buttonMap.setSize(_w, _h);
	menuview->updatePosition();
}

void Menu::receiveEvent(Event *ev) {
	if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)
			game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC)
			game->stateFinished(this, Returny(Returny::empty_returny));
	}
}

void Menu::buttonClick(Button *btn) {
	if (btn == startlevel_btn)
		startLevel("Data/level1.lua");
}

void Menu::startLevel(std::string path) {
	std::cout << "starting level " << path << std::endl;
	level = new Level(game, theW, path);		// Need error handling here (e.g. for exception in Level constructor?)
	game->pushState(level);
}


#include "../W.hpp"

MenuView::MenuView(W *_theW, JenniferAniston &_aniston, ResponderMap *_buttonMap, std::vector<Button*> *_buttons)
: View(_theW, _aniston), buttonMap(_buttonMap), buttons(_buttons)
{
	
}
void MenuView::draw() {
	for (int i=0, n = buttons->size(); i < n; i++) {
		Button *b = (*buttons)[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
}
void MenuView::processMouseEvent(Event *ev) {
	buttonMap->dispatchEvent(ev);
}
