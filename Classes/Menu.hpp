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

#include "GameState.hpp"
#include "EventResponder.hpp"

class Game;
class W;
class Level;
class Button;

class MenuBackgroundView;
class ClickToBeginView;

class Menu : public GameState, public EventResponder {
public:
	Menu(Game *, W *);
	~Menu();
	
	// Methods
	void reset();
	void pause();
	void resume(Returny *);
	void update();
	void draw();
	void setResolution(int _w, int _h);
	
	void receiveEvent(Event *);
	
	void startLevel(std::string path);
	void startLevelOne();
	
protected:
	// Properties
	MenuBackgroundView *menubackgroundview;
	ClickToBeginView *clicktobeginview;
	Level *level;		// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	
};


#include "View.hpp"

class ClickToBeginView : public View {
public:
	ClickToBeginView(W *, JenniferAniston &, Menu *);
	~ClickToBeginView();
	void draw();
	void processMouseEvent(Event *);
private:
	bool mr_hov;
	Menu *menu;
};


struct colour_square;

class MenuBackgroundView : public View {
public:
	MenuBackgroundView(W *, JenniferAniston &);
	~MenuBackgroundView();
	void draw();
private:
	int *colour_squares;
	__colour *colour_cycle_array;
	int      colour_cycle_length;
	
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
