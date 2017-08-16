#include "LevelScore.hpp"
#include "Level.hpp"
#include "MrKlangy.hpp"

LevelScore::LevelScore(W::Window *_win, bool _victory) : win(_win), victory(_victory)
{
	scoreview = new ScoreView(win, &eh, victory);
	addView(scoreview);
	scoreview->subscribeToButtons(new W::Callback(&LevelScore::recEv, this));
	
	if (!victory) MrKlangy::playSound("pacman-death.wav");
	MrKlangy::playBGM(victory ? "win.ogg" : "lose-b.ogg", false);

	// Key subscriptions
	eh.subscribeToEventType(W::EventType::KEYPRESS, W::Callback(&LevelScore::recEv, this));
}
LevelScore::~LevelScore()
{
	removeView(scoreview);
	delete scoreview;
	
	MrKlangy::stopBGM();
}

void LevelScore::resume(W::Returny *returny) { }
void LevelScore::update() { }

void LevelScore::recEv(W::Event *ev) {
	if (ev->type == W::EventType::KEYPRESS) {
		if (ev->key == W::KeyCode::ESC) exitToMenu();
	}
	else if (ev->type == W::EventType::BUTTONCLICK) {
		if (*((std::string*)ev->_payload) == "exit to menu") exitToMenu();
		else if (*((std::string*)ev->_payload) == "replay")  replayLevel();
	}
	std::cout << "levelscore: recev finished" << std::endl;
}

void LevelScore::exitToMenu() {
	W::Returny ret(W::ReturnyType::PAYLOAD_RETURNY);
	ret.payload = "exit to menu";
	printf("LevelScore (%p) calling popState with payload \"exit to menu\"...\n", this);
	W::popState(ret);
	std::cout << "levelscore: exit to menu finisehd" << std::endl;
}
void LevelScore::replayLevel() {
	W::Returny ret(W::ReturnyType::PAYLOAD_RETURNY);
	ret.payload = "replay";
	printf("LevelScore (%p) calling popState with payload \"replay\"...\n", this);
	W::popState(ret);
	std::cout << "levelscore: popstate called successfully" << std::endl;
}


ScoreView::ScoreView(W::Window *_win, W::EventHandler *_eh, bool _victory) :
	UIView(
		new W::Positioner(W::TOP_LEFT, W::PFIXED, W::PPROPORTIONAL, 0, 0, 1, 1),
		_win,
		_eh
	),
	victory(_victory)
{
	W::rect r = {
		W::position(240, 155),
		W::size(120, 80)
	};
	W::rect r2 = {
		W::position(440, 155),
		W::size(120, 80)
	};
	buttons.push_back(new W::Button(r, "replay"));
	buttons.push_back(new W::Button(r2, "exit to menu"));
}
void ScoreView::draw() {
	for (std::vector<W::Button*>::iterator it = buttons.begin(); it < buttons.end(); it++) {
		W::Button *b = *it;
		drawRect(b->pos.x, b->pos.y, b->plan[0].sz.width, b->plan[0].sz.height, b->col());
	}
	drawText(260, 250, W::Colour::Black, (char*)"replay");
	drawText(474, 250, W::Colour::Black, (char*)"menu");
	if(victory) drawText(340, 88, W::Colour::White, (char*)"YOU WIN!");
	else        drawText(346, 88, W::Colour::White, (char*)"YOU LOSE!");
}
