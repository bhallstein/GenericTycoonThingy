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
	
	if (!victory) theW->playSound("pacman-death.wav");
	theW->playBGM(victory ? "win.ogg" : "lose-b.ogg");

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
		if (ev->key == Event::K_ESC) {
			exitToMenu();
		}
	}
	else if (ev->type == Event::BUTTONCLICK) {
		if (ev->payload == "exit to menu") exitToMenu();
		else if (ev->payload == "replay") replayLevel();
	}
}

void LevelScore::exitToMenu() {
	Returny ret = Returny(Returny::payload_returny);
	ret.payload = "exit to menu";
	game->stateFinished(this, ret);
}
void LevelScore::replayLevel() {
	Returny ret = Returny(Returny::payload_returny);
	ret.payload = "replay";
	game->stateFinished(this, ret);
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
	buttons.push_back(new Button(240, 155, 120, 80, "replay"));
	buttons.push_back(new Button(440, 155, 120, 80, "exit to menu"));
	victory = _victory;
}
void ScoreView::draw() {
	for (int i=0, n = buttons.size(); i < n; i++) {
		Button *b = buttons[i];
		theW->drawRect(b->x, b->y, b->width, b->height, b->col());
	}
	theW->drawText(260, 250, "black", (char*)"replay");
	theW->drawText(474, 250, "black", (char*)"menu");
	if(victory)
		theW->drawText(340, 88, "white", (char*)"YOU WIN!");
	else
		theW->drawText(346, 88, "white", (char*)"YOU LOSE!");
}
