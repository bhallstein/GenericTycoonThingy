#include "LevelScore.hpp"
#include "Game.hpp"
#include "Level.hpp"
#include "Button.hpp"
#include "Callback.hpp"

LevelScore::LevelScore(Game *_game, W *_theW, bool _victory) : GameState(_game, _theW)
{
	JenniferAniston aniston(theW, TOP_LEFT, PFIXED, PPROPORTIONAL, 0, 0, 1, 1);
	scoreview = new ScoreView(_theW, aniston, &responderMap, _victory);
	responderMap.addResponder(scoreview);
	
	scoreview->subscribeToButtons(new Callback(&LevelScore::receiveEvent, this));
	
	victory = _victory;
	
	theW->playSound(victory ? "Mission accomplished.wav" : "Mission failed.wav");
	theW->playBGM(victory ? "win.ogg" : "lose.ogg");

	// Key subscriptions
	responderMap.subscribeToKey(this, Event::K_Q);
	responderMap.subscribeToKey(this, Event::K_ESC);
}
LevelScore::~LevelScore()
{	
	delete scoreview;
}

void LevelScore::pause() {
	
}
void LevelScore::resume(Returny *returny) {
	delete level;
	if (returny->type == Returny::killer_returny)
		game->stateFinished(this, Returny(Returny::killer_returny));
}
void LevelScore::update() {

}
void LevelScore::draw() {
	scoreview->_draw();
}

void LevelScore::setResolution(int _w, int _h) {
	GameState::setResolution(_w, _h);
	scoreview->updatePosition();
}

void LevelScore::receiveEvent(Event *ev) {
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

void LevelScore::startLevel(std::string path) {
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

ScoreView::ScoreView(W *_theW, JenniferAniston &_aniston, ResponderMap *_rm, bool _victory) : UIView(_theW, _aniston, _rm, DISALLOW_DRAG)
{
	//buttons.push_back(new Button(280, 160, 240, 110, "start level 1"));
	victory = _victory;
}
void ScoreView::draw() {
	/*for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}*/
	if(victory)
		theW->drawText(180, 180, "green", (char*)"YOU WIN!");
	else
		theW->drawText(180, 180, "red", (char*)"YOU LOSE!");
}
