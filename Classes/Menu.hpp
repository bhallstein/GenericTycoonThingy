/*
 * Menu.hpp - a menu class
 * 
 * This will presumably be a base class used for creating specific menus.
 * Just implementing a single menu now, to find out what it might entail.
 *
 */

#ifndef MENU_H
#define MENU_H

#include <vector>
#include "W.h"

class MenuBackgroundView;
class ClickToBeginView;

class Level;

class Menu : public W::GameState, public W::EventResponder {
public:
	Menu(W::Window *);
	~Menu();
	
	// Methods
	void resume(W::Returny *);
	void update();
	
	void receiveEvent(W::Event *);
	
	void startLevel(std::string path);
	void startLevelOne();
	
protected:
	MenuBackgroundView *menubackgroundview;
	ClickToBeginView *clicktobeginview;

	Level *level;		// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	W::Window *win;
};

class ClickToBeginView : public W::UIView {
public:
	ClickToBeginView(W::Window *, W::EventHandler *, Menu *);
	void draw();
	void processMouseEvent(W::Event *);
private:
	bool mr_hov;
	Menu *menu;
};


struct colour_square;

class MenuBackgroundView : public W::View {
public:
	MenuBackgroundView(W::Window *);
	~MenuBackgroundView();
	void draw();
private:
	int *colour_squares;
	W::Colour *colour_cycle_array;
	int colour_cycle_length;
	
	int framecount;
	
	enum _modes {
		MNONE,
		MRANDOM, MSTRIPESH, MSTRIPESREV, MFADE, /*MSTRIPESD,*/ MPULSE, /*MTURNY,*/
		MTRANSWIPE, MTRANSCIRC, MTRANSBARS,
		MLAST
	} mode;
	void switchMode();
	
	void incrementColour(int *sq, int n = 1) {
		*sq += n;
		if (*sq >= colour_cycle_length) *sq -= colour_cycle_length;
	}
	void decrementColour(int *sq, int n = 1) {
		*sq -= n;
		if (*sq < 0) *sq += colour_cycle_length;
	}
	
	float alpha;
	bool mrandom_prerandomised;
	Menu *menu;
	int *mtransbars_heights;
};

#endif
