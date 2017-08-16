#include "LevelScore.hpp"
#include "Level.hpp"

LevelScore::LevelScore(W::Window *_win, bool _victory) : win(_win), victory(_victory)
{
	scoreview = new ScoreView(win, &eh, victory);
	addView(scoreview);
	scoreview->subscribeToButtons(new W::Callback(&LevelScore::recEv, this));
	
//	theW->playSound(victory ? "Mission accomplished.wav" : "Mission failed.wav");

	// Key subscriptions
	eh.subscribeToKey(W::KeyCode::K_Q, new W::Callback(&LevelScore::recEv, this));
	eh.subscribeToKey(W::KeyCode::K_ESC, new W::Callback(&LevelScore::recEv, this));
}
LevelScore::~LevelScore()
{	
	delete scoreview;
}

void LevelScore::resume(W::Returny *returny) { }
void LevelScore::update() { }

void LevelScore::recEv(W::Event *ev) {
	if (ev->type == W::EventType::KEYPRESS) {
		if (ev->key == W::KeyCode::K_ESC) exitToMenu();
	}
	else if (ev->type == W::EventType::BUTTONCLICK) {
		if (*((std::string*)ev->_payload) == "exit to menu") exitToMenu();
		else if (*((std::string*)ev->_payload) == "replay") replayLevel();
	}
}

void LevelScore::exitToMenu() {
	W::Returny ret(W::ReturnyType::PAYLOAD_RETURNY);
	ret.payload = "exit to menu";
	W::popState(ret);
}
void LevelScore::replayLevel() {
	W::Returny ret(W::ReturnyType::PAYLOAD_RETURNY);
	ret.payload = "replay";
	W::popState(ret);
}


ScoreView::ScoreView(W::Window *_win, W::EventHandler *_eh, bool _victory) :
	UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		_win,
		_eh
	),
	victory(_victory)
{
	buttons.push_back(new W::Button(240, 155, 120, 80, "replay"));
	buttons.push_back(new W::Button(440, 155, 120, 80, "exit to menu"));
}
void ScoreView::draw() {
	for (std::vector<W::Button*>::iterator it = buttons.begin(); it < buttons.end(); it++) {
		W::Button *b = *it;
		drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
	if (victory) drawText(180, 180, W::Colour::GREEN, (char*)"YOU WIN!");
	else drawText(180, 180, W::Colour::RED, (char*)"YOU LOSE!");
}
