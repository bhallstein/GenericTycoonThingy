#include "Menu.hpp"

Menu::Menu(Game *_game, sf::RenderWindow *_window) : GameState(_game, _window)
{
	menuview = new MenuView(window, 16, 16, 0, 0, 0, 0);
	menuview->createEventResponderMap();
	eventHandler.subscribe(menuview);
	
	// Add buttons, add them to menuview
	Button *b = new Button(this, 3, 3);			// For menu customization, could set various this-stuffery from xml shiz.
	buttons.push_back(b);						// 
	menuview->addResponder(b);					//
}
Menu::~Menu()
{
	// Remove buttons form view
	Button *b;
	for (int i=0, n = buttons.size(); i < n; i++) {
		b = buttons[i];
		menuview->removeResponder(b);
		delete b;
	}
	buttons.clear();
	
	delete menuview;
}

void Menu::reset() {
	
}
void Menu::pause() {
	
}
void Menu::resume() {
	
}
void Menu::handleEvent(Event *ev) {
	// Handle things like ESC, UP, DOWN, ENTER...
}
void Menu::update() {

}
void Menu::draw() {
	menuview->draw(&buttons);
}

void Menu::startLevel(std::string path) {
	level = new Level(game, window, path);		// Urgently need error handling here (exception in Level constr?)
	game->pushState(level);
}


Button::Button(Menu *_menu, int _x, int _y) : menu(_menu) {
	x = _x, y = _y;
	w = 4, h = 2;
}
Button::~Button() {
	std::cout << "Button destruct" << std::endl;
}
void Button::receiveEvent(Event *ev) {
	if (ev->type == MOUSEMOVE)
		hovered = true;
		
	else if (ev->type == LEFTCLICK)
		menu->startLevel("Data/level1.xml");
}
char Button::col() {
	bool hov = hovered;
	hovered = false;
	return hov ? 'r' : 'b';
}

MenuView::MenuView(sf::RenderWindow *_win, int _blocks_w, int _blocks_h, int _l_offset, int _t_offset, int _r_offset, int _b_offset) :
	View(_win, _blocks_w, _blocks_h, _l_offset, _t_offset, _r_offset, _b_offset)
{
	
}
void MenuView::draw(std::vector<Button*> *buttons) {
	for (std::vector<Button*>::iterator i = buttons->begin(); i < buttons->end(); i++)
		drawRect(
			(*i)->col() == 'r' ? sf::Color::Red : sf::Color::Blue,
			(*i)->x, (*i)->y, (*i)->w, (*i)->h
		);
}