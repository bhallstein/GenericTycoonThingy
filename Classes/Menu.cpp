#include "Menu.hpp"
#include "Level.hpp"
#include "MrKlangy.hpp"

Menu::Menu(W::Window *_win) : W::GameState(W::GS_OPAQUE), win(_win)
{
	menubackgroundview = new MenuBackgroundView(win);
	addView(menubackgroundview);
	
	clicktobeginview = new ClickToBeginView(win, &eh, this);
//	clicktobeginview->subscribeToButtons(new W::Callback...)
	addView(clicktobeginview);
	
	// Key subscriptions
	eh.subscribeToKey(W::KeyCode::K_Q, W::Callback(&Menu::receiveEvent, this));
	eh.subscribeToKey(W::KeyCode::K_ESC, W::Callback(&Menu::receiveEvent, this));

	MrKlangy::playBGM("menu.xm");
}
Menu::~Menu()
{
	removeView(menubackgroundview); delete menubackgroundview;
	removeView(clicktobeginview);   delete clicktobeginview;
	MrKlangy::stopBGM();
}

void Menu::resume(W::Returny *ret) {
	printf("Menu::resume\n");
	MrKlangy::playBGM("menu.xm");
//	delete level;
	if (ret->type == W::ReturnyType::KILLER_RETURNY)
		W::popState(W::KillerReturny);
	else if (ret->type == W::ReturnyType::PAYLOAD_RETURNY) {
		if (ret->payload == "replay") startLevel("level1.lua");
	}
	printf("Menu::resume finished\n");
}
void Menu::update() {
	
}

void Menu::receiveEvent(W::Event *ev) {
	if (ev->type == W::EventType::KEYPRESS) {
		if (ev->key == W::KeyCode::K_Q || ev->key == W::KeyCode::K_ESC)
			W::popState(W::EmptyReturny);
	}
//	else if (ev->type == W::EventType::BUTTONCLICK) {
//		if (*((std::string*) ev->_payload) == "start level 1")
//			startLevel("level1.lua");
//	}
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


ClickToBeginView::ClickToBeginView(W::Window *_win, W::EventHandler *_eh, Menu *_menu) :
	W::UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		_win, _eh
	),
	menu(_menu)
{
	mr_hov = false;
}
void ClickToBeginView::draw() {
	if (mr_hov) drawRect(160, 224, 480, 100, W::Colour(1,0,0,0.7), 0), mr_hov = false;
	else drawRect(160, 224, 480, 100, W::Colour(0,0,0,0.2), 0);
	//	for (int i=0, n = buttons.size(); i < n; i++) {
	//		Button *b = buttons[i];
	//		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	//	}
	drawText(186, 254, W::Colour::Black, (char *) "Welcome to Generic Tycoon Thingy");
	drawText(308, 284, W::Colour::White, (char *) "Click to begin");
}
void ClickToBeginView::processMouseEvent(W::Event *ev) {
	if (ev->pos.x >= 160 && ev->pos.x < 160 + 480 && ev->pos.y >= 224 && ev->pos.y < 224 + 100) {
		if (ev->type == W::EventType::MOUSEMOVE) mr_hov = true;
		else if (ev->type == W::EventType::LEFTMOUSEDOWN) menu->startLevel("level1.lua");
	}
}


#include <cmath>

MenuBackgroundView::MenuBackgroundView(W::Window *_win) :
	W::View(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		_win
	)
{	
	colour_cycle_length = 60;
	colour_cycle_array = (W::Colour*) malloc(sizeof(W::Colour) * colour_cycle_length);
	for (int i=0; i < colour_cycle_length; i++) {
		W::Colour *c = &colour_cycle_array[i];
		c->r = (i < 10 || i >= 50) ? 1 : (i < 20) ? (20-i)/10.0 : (i >= 40) ? (i-40)/10.0 : 0;
		c->g = (i < 10) ? i/10.0 : (i < 30) ? 1 : (i < 40) ? (40-i)/10.0 : 0;
		c->b = (i < 20) ? 0 : (i < 30) ? (i-20)/10.0 : (i < 50) ? 1 : (60-i)/10.0;
		c->a = 1;
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
	int width = plan[0].sz.width, height = plan[0].sz.height;
	for (int i=0; i < 40 * 30; i++) {
		int _x = i%40 * width/40, _y = i/40 * height/30;
		drawRect(_x, _y, width/40, height/30, colour_cycle_array[colour_squares[i]], false);
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
