/*
 * LevelScore.hpp - a gamestate
 * 
 * This is a state for displaying victory/failure and associated score at the end of a level
 *
 */

#ifndef LEVELSCORE_H
#define LEVELSCORE_H

#include <vector>

#include "GameState.hpp"
#include "EventResponder.hpp"

class Game;
class W;
class Level;
class Button;

class ScoreView;

class LevelScore : public GameState, public EventResponder {
public:
	LevelScore(Game *, W *, bool);
	~LevelScore();
	
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
	
	void exitToMenu();
	void replayLevel();
	
protected:
	// Properties
	ScoreView *scoreview;
	Level *level;		// This (or a future LevelLoadinator) will clearly one day be specific to a subclass
	bool victory;
	
};


#include "UIView.hpp"

class ScoreView : public UIView {
public:
	ScoreView(W *, JenniferAniston &, ResponderMap *, bool _victory);
	void draw();
	bool victory;
};

#endif
