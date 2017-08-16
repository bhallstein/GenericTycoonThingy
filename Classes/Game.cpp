#include "Game.hpp"
#include "../W.hpp"
#include "SettingsManager.hpp"
#include "Menu.hpp"

Game::Game(W *_theW) : theW(_theW), resume(false), finishedIntro(false)
{
	int game_stage = DEMON_STAGE;	// Should be a setting or otherwise disk-saved (perhaps in its own file)
	
	settings = new SettingsManager(theW);
	if (settings->fullscreen.value)	{ }	// The value of this somehow seems to be getting *randomly generated* at
										// startup, at present. :S
	theW->goWindowed();
	theW->setWindowTitle(game_stage == TUTORIAL_STAGE ? "Happy Hair Tycoon" : "Demon Barber Tycoon");
	prev_w = theW->width(), prev_h = theW->height();
//	theW->setDefaultWindowSize(settings->Screen_Width.value, settings->Screen_Height.value);
}

Game::~Game()
{
	delete settings;
}

void Game::sendEvents(std::vector<Event> *events) {
	for (std::vector<Event>::iterator it = events->begin(); it != events->end(); it++)
		states.back()->handleEvent(&(*it));
}

void Game::pushState(GameState *st)
{
	for (int i=0, n = states.size(); i < n; i++)
		if (states[i] == st)
			return;									// Throw exception if state already in stack?
		
	if (!states.empty()) states.back()->pause();	// Pause current state
	states.push_back(st);							// Add the new state
}

void Game::popState()
{
	states.pop_back();
	if (!states.empty()) states.back()->resume(&returny);
}
void Game::stateFinished(GameState *state, Returny _returny) {
	if (states.back() != state)	return;				// Throw exception
	resume = true;
	returny = _returny;
}

bool Game::update()	{		// Return false to quit
	// Switch to base menu
	// pushState(new Menu(this, window));
	
	if (states.empty()) {
		if (finishedIntro)
			return false;
		else
			finishedIntro = true, pushState(new Menu(this, theW));
	}
	
	if (resume) {
		resume = false;
		popState();					// popState() resumes previous state, which may call stateFinished(),
	}								// thus setting resume back to true.
	if (resume) return true;
	if (states.empty())	return false;
	
	sendEvents(theW->getEvents());	// events
	states.back()->update();		// update current gamestate	

	return true;
}

void Game::draw() {
	int n = states.size();
	
	int w = theW->width(), h = theW->height();
	if (w != prev_w || h != prev_h) {
		prev_w = w, prev_h = h;
		for (int i = 0; i < n; i++)
			states[i]->setResolution(w, h);
	}
	
	int i, first_to_draw = n - 1;
	for (i = n-1; i >= 0; i--)
		if (states[i]->translucent)
			first_to_draw = (i ? i-1 : 0);
	for (i = first_to_draw; i < n; i++)
		states[i]->draw();
}
