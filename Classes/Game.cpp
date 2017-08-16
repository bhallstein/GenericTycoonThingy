#include "Game.hpp"

Game::Game(sf::RenderWindow *_window, SettingsManager* _settings) : resume(false)
{
	//point our settings property to the SettingsManager instance
	settings = _settings;

	int game_stage = DEMON_STAGE;	// Should be a setting or otherwise disk-saved (perhaps in its own file?)
	
	// Window setup
	window = _window;
	/* 
	 * Problem: at some point the window may be created in platform-specific code, wrapped in a C++ class
	 * (say, 'W'), to provide a unified interface, and, thus, most importantly – outside the scope of Game.
     * 
	 * This leads to the necessity to do one of the following:
	 *    1. to separate window-related settings into a different settings file,
	 *       loaded before we enter Game scope
	 *    2. for the W class to have the capability to switch itself on command between windowed and fullscreen modes
	 * 
	 * 2. is probably better – it enables us to consolidate settings into the game code. And it is 
	 * more flexible: the game should be able most excellent-like to switch graphics modes on the fly.
	 */
	int windowStyle = (settings->Windowed.value ? sf::Style::Close : sf::Style::Fullscreen);
	std::string window_name = (game_stage == TUTORIAL_STAGE ? "Happy Hair Tycoon" : "Demon Barber Tycoon");
 	window->Create(sf::VideoMode(settings->Screen_Width.value, settings->Screen_Height.value), window_name, windowStyle);
	window->SetFramerateLimit(settings->FramerateLimit.value);
}

Game::~Game()
{
	
}

void Game::sendEvents() {
	Event event;
	sf::Event sf_event;
	
	// Generate our own mousemoves
	event.loadFromMousePos(sf::Mouse::GetPosition(*window));
	if (event.x > 0 && event.x < window->GetWidth() && event.y > 0 && event.y < window->GetHeight())
		states.back()->_handleEvent(&event);
		
	while (window->PollEvent(sf_event)) {
		event.loadFromSFEvent(&sf_event);			// Convert sfml event to our own type
		if (event.type == MOUSEMOVE) continue;		// Ignore sfml mouse moves
		states.back()->_handleEvent(&event);		// Send keys to active state
	}
}
void Game::update()
{
	// Update the active state
	states.back()->update();
}
void Game::draw()
{
	// Clear screen
	window->Clear(sf::Color(138,43,226));		// Electric Indigo, bitches

	// Draw state(s)
	int i, n = states.size(), first_to_draw = n - 1;
	for (i = n-1; i >= 0; i--)
		if (states[i]->translucent)
			first_to_draw = (i ? i-1 : 0);
	for (i = first_to_draw; i < n; i++)
		states[i]->draw();

	// Refresh the window
	window->Display();
}

void Game::pushState(GameState *st)
{
	for (int i=0, n = states.size(); i < n; i++)
		if (states[i] == st)
			; 										// Throw exception if state already in stack
		
	if (!states.empty()) states.back()->pause();	// Pause current state

	// Add the new state
	states.push_back(st);
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

void Game::run()
{	
	// Switch to base menu
	pushState(new Menu(this, window));
	
	/* What Game really wants to do is run a sequence of initial states:
	   these might include a splash, video, and so on. i.e. it will need to run through a list:
	   if (something remains in the list), push it; otherwise, quit. */
	
	while (!states.empty())
    {
		if (resume) {
			resume = false;					// popState() resumes previous state: it may call stateFinished(),
			popState();						// setting resume to TRUE
			if (states.empty() || resume)
				continue;
		}
		sendEvents();
		update();
		draw();
    }

}
