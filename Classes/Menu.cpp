#include "Menu.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Button.hpp"
#include "Callback.hpp"

Menu::Menu(Game *_game, W *_theW) : GameState(_game, _theW)
{
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	menubackgroundview = new MenuBackgroundView(theW, aniston);
	
	JenniferAniston aniston2(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	clicktobeginview = new ClickToBeginView(theW, aniston2, this);
	responderMap.addResponder(clicktobeginview);
	
//	menuview->subscribeToButtons(new Callback(&Menu::receiveEvent, this));
	
	// Key subscriptions
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_ESC);

	theW->playBGM("menu.xm");
}
Menu::~Menu()
{	
	delete menubackgroundview;
}

void Menu::pause() {
	
}
void Menu::resume(Returny *returny) {
	theW->playBGM("menu.xm");
	delete level;
	if (returny->type == Returny::killer_returny)
		game->stateFinished(this, Returny(Returny::killer_returny));
	else if (returny->type == Returny::payload_returny)
		startLevel("level1.lua");
}
void Menu::update() {

}
void Menu::draw() {
	menubackgroundview->_draw();
	clicktobeginview->_draw();
}

void Menu::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	menubackgroundview->updatePosition();
}

void Menu::receiveEvent(Event *ev) {
	if (ev->type == Event::KEYPRESS) {
		if (ev->key == Event::K_Q)
			game->stateFinished(this, Returny(Returny::killer_returny));
		if (ev->key == Event::K_ESC)
			game->stateFinished(this, Returny(Returny::empty_returny));
	}
//	else if (ev->type == Event::BUTTONCLICK) {
//		if (ev->payload == "start level 1")
//			startLevel("level1.lua");
//	}
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

ClickToBeginView::ClickToBeginView(W *_theW, JenniferAniston &_aniston, Menu *_menu) :
	View(_theW, _aniston), menu(_menu)
{
	mr_hov = false;
}
void ClickToBeginView::draw() {
	// Main
	if (mr_hov) theW->drawRect(160, 224, 480, 100, "red", 0, 0.7), mr_hov = false;
	else theW->drawRect(160, 224, 480, 100, "black", 0, 0.2);
	//	for (int i=0, n = buttons.size(); i < n; i++) {
	//		Button *b = buttons[i];
	//		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	//	}
	theW->drawText(186, 254, "black", (char *) "Welcome to Generic Tycoon Thingy");
	theW->drawText(308, 284, "white", (char *) "Click to begin");
}
void ClickToBeginView::processMouseEvent(Event *ev) {
	if (ev->type == Event::MOUSEMOVE) {
		if (ev->x >= 160 && ev->x < 160 + 480 && ev->y >= 224 && ev->y < 224 + 100)
			mr_hov = true;
	}
	else if (ev->type == Event::LEFTCLICK)
		if (mr_hov)
			menu->startLevel("level1.lua");
}


#include <cmath>

MenuBackgroundView::MenuBackgroundView(W *_theW, JenniferAniston &_aniston) : View(_theW, _aniston)
{	
	colour_cycle_length = 60;
	colour_cycle_array = (__colour*) malloc(sizeof(__colour) * colour_cycle_length);
	for (int i=0; i < colour_cycle_length; i++) {
		__colour *c = &colour_cycle_array[i];
		c->red   = (i < 10 || i >= 50) ? 1 : (i < 20) ? (20-i)/10.0 : (i >= 40) ? (i-40)/10.0 : 0;
		c->green = (i < 10) ? i/10.0 : (i < 30) ? 1 : (i < 40) ? (40-i)/10.0 : 0;
		c->blue  = (i < 20) ? 0 : (i < 30) ? (i-20)/10.0 : (i < 50) ? 1 : (60-i)/10.0;
		c->alpha = 1;
	}
	
	colour_squares = (int*) malloc(sizeof(int) * 40 * 30);
	
	framecount = 0;
	alpha = 0;
	mode = MNONE;
	mrandom_prerandomised = false;
	mtransbars_heights = (int*) malloc(sizeof(int) * 40);
}
MenuBackgroundView::~MenuBackgroundView() {
	free(colour_squares);
	free(colour_cycle_array);
	free(mtransbars_heights);
}

void MenuBackgroundView::draw() {
	// Swirly colours
	
	if (alpha < 0.8) alpha += 0.008;
	
	if (mode == MNONE || ++framecount > 100)
		switchMode();
	else if (mode == MRANDOM || /*mode == MSTRIPESD ||*/ mode == MFADE || mode == MPULSE)
		for (int i=0; i < 40 * 30; i++) incrementColour(&colour_squares[i], 1);
	else if (mode == MSTRIPESH) for (int i=0; i < 40*30; i++) incrementColour(&colour_squares[i], 2);
	else if (mode == MSTRIPESREV) for (int i=0; i < 40*30; i++) decrementColour(&colour_squares[i], 2);
//	else if (mode == MTURNY) {
//		float angle = framecount/10.0;
//		int cx = 20, cy = 15;
//		float diff = 0.2;
//		for (float _x = cx, _y = cy;
//			 _x >= 0 && _y >= 0 && _x < 40 && _y < 30;
//			 _x -= diff * sin(angle), _y -= diff * cos(angle)) {
//			int _xx = _x, _yy = _y;
//			colour_squares[_yy * 40 + _xx] = W::randUpTo(colour_cycle_length);
//		}
//		for (int i=0; i < 40*30; i++) incrementColour(&colour_squares[i]);
//	}
	else if (mode == MTRANSWIPE) {
		int _x = 40 - framecount;
		if (_x >= 0)
			for (int y=0; y < 30; y++)
				colour_squares[y*40+_x] = W::randUpTo(colour_cycle_length);			
		for (int i=0; i < 40*30; i++) incrementColour(&colour_squares[i]);
	}
	else if (mode == MTRANSCIRC) {
		int outer_radius = 40 - framecount, inner_radius = outer_radius - 2;
		for (int i=0; i < 40*30; i++) {
			int _x = i%40-20, _y = i/40-15;
			int dist = sqrt((double)(_x*_x + _y*_y));
			if (dist < outer_radius && dist >= inner_radius)
				colour_squares[i] = W::randUpTo(colour_cycle_length);
			incrementColour(&colour_squares[i]);
		}
	}
	else if (mode == MTRANSBARS) {
		for (int _x=0; _x < 40; _x++) {
			int _y = framecount - mtransbars_heights[_x] - 1;
			if (_y >= 0 && _y < 30)
				colour_squares[_y*40+_x] = W::randUpTo(colour_cycle_length);
		}
		for (int i=0; i < 40*30; i++) incrementColour(&colour_squares[i]);
	}
	
	// Draw
	for (int i=0; i < 40 * 30; i++) {
		int _x = i%40 * width/40, _y = i/40 * height/30;
		theW->drawRect(_x, _y, width/40, height/30, &colour_cycle_array[colour_squares[i]], false, alpha);
	}
}
void MenuBackgroundView::switchMode() {
	framecount = 0;
	if (mode == MTRANSCIRC || mode == MTRANSWIPE || mode == MTRANSBARS)
		mode = MRANDOM;
	else {
		int m = (_modes) mode;
		mode = (_modes) ++m;
		if (mode == MLAST) mode = (_modes) 1;
		if (mode == MTRANSWIPE) {
			int r = W::randUpTo(3);
			switch (r) {
				case 0 : mode = MTRANSWIPE; break;
				case 1 : mode = MTRANSCIRC; break;
				case 2 : mode = MTRANSBARS; break;
			}	
		}
	}
	// setupses
	if (mode == MRANDOM) {
		if (mrandom_prerandomised) return;
		for (int i=0; i < 40 * 30; i++)
			colour_squares[i] = W::randUpTo(colour_cycle_length);
		mrandom_prerandomised = true;
	}
	else if (mode == MSTRIPESH) {
		for (int x=0; x < 40 + 30; x++)
			for (int xx = x, yy = 0; xx >= 0 && yy < 30; xx--, yy++)
				if (xx >= 40) continue;
				else colour_squares[yy * 40 + xx] = xx%colour_cycle_length;
	}
	else if (mode == MFADE) {
		int _c = W::randUpTo(colour_cycle_length);
		for (int i=0; i < 40*30; i++)
			colour_squares[i] = _c;
	}
//	else if (mode == MSTRIPESD) {
//		for (int x=0; x < 40 + 30; x++)
//			for (int xx = x, yy = 0; xx >= 0 && yy < 30; xx--, yy++)
//				if (xx >= 40) continue;
//				else colour_squares[yy * 40 + xx] = x%colour_cycle_length;
//	}
	else if (mode == MPULSE) {
		for (int i=0; i < 40*30; i++) {
			int _x = i%40 - 20, _y = i/40 - 15;
			int _h = sqrt((double) (_x*_x + _y*_y));
			colour_squares[i] = _h%colour_cycle_length;
		}
	}
	else if (mode == MTRANSWIPE) {
		
	}
	else if (mode == MTRANSCIRC) {
		
	}
	else if (mode == MTRANSBARS) {
		for (int i=0; i < 40; i++) mtransbars_heights[i] = W::randUpTo(15);
	}
//	else if (mode == MTURNY) {
//		
//	}
}
