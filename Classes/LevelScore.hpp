/*
 * LevelScore.hpp - a gamestate
 * 
 * This is a state for displaying victory/failure and associated score at the end of a level
 *
 */

#ifndef LEVELSCORE_H
#define LEVELSCORE_H

#include <vector>
#include "W.h"

class Level;
class ScoreView;

class LevelScore : public W::GameState, public W::EventResponder {
public:
	LevelScore(W::Window *, bool _victory);
	~LevelScore();
	
	// Methods
	void resume(W::Returny *);
	void update();
	
	void recEv(W::Event *);
	
	void exitToMenu();
	void replayLevel();
	
protected:
	// Properties
	ScoreView *scoreview;
	bool victory;
	W::Window *win;
	
};

class ScoreView : public W::UIView {
public:
	ScoreView(W::Window *_win, W::EventHandler *_eh, bool _victory);
	void draw();
	bool victory;
};

#endif
