#include "GameState.hpp"
#include "Game.hpp"
#include "../W.hpp"
#include "Returny.hpp"

GameState::GameState(Game *_game, W *_theW) :
	game(_game), theW(_theW), translucent(false), responderMap(_theW->width(), _theW->height())
{
	
}
GameState::~GameState() {
	
}

void GameState::handleEvent(Event *ev) {
	if (ev->type == Event::CLOSED)
		handleCloseEvent();
	else
		responderMap.dispatchEvent(ev);
}
void GameState::handleCloseEvent() {
	game->stateFinished(this, Returny(Returny::killer_returny));
}

void GameState::setResolution(int _w, int _h) {
	responderMap.setSize(_w, _h);
}
