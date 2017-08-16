#include "Game.hpp"
#include "Level.hpp"

Game::Game(sf::RenderWindow *_window, SettingsManager* _settings)
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
	std::string window_name = game_stage == TUTORIAL_STAGE ? "Happy Hair Tycoon" : "Demon Barber Tycoon";
 	window->Create(sf::VideoMode(settings->Screen_Width.value, settings->Screen_Height.value), window_name, windowStyle);
	window->SetFramerateLimit(settings->FramerateLimit.value);
}

Game::~Game()
{
	
}

void Game::cleanStates()
{
	// Clean up all the states
	while ( !states.empty() ) {
		states.back()->reset();
		states.pop_back();
	}
}

//Events/Update/Draw - usually passed to active State
void Game::handleEvents(Event event,sf::Event sf_event)
{
	event.loadFromMousePos(sf::Mouse::GetPosition(*window));					
	if (event.x > 0 && event.x < window->GetWidth() && event.y > 0 && event.y < window->GetHeight())
		eventHandler.dispatchEvent(&event);										
		
	while (window->PollEvent(sf_event)) {
		event.loadFromSFEvent(&sf_event);		// Convert sfml event to our own type
			
		// Close window : exit
		if (event.type == CLOSED || event.key == K_Q)
			quit();
		
		else
			states.back()->handleEvents(this, &event);	// Pass to active state
	}
}

void Game::update()
{
	//let the current active state perform updates
	states.back()->update(this);
}
void Game::draw()
{
	//Clear screen
	window->Clear(sf::Color(138,43,226));		// Electric Indigo, bitches

	//let the current active state draw
	states.back()->draw(this);

	//Display shizzle
	window->Display();	// Refresh the window
}

void Game::changeState(GameState *state)
{
	// Destroy the current state
	if ( !states.empty() ) {
		states.back()->reset();
		states.pop_back();
	}

	// Add the new state
	states.push_back(state);
	states.back()->init(window, &eventHandler);
}

void Game::pushState(GameState* state)
{
	// Pause current state
	if ( !states.empty() ) {
		states.back()->pause();
	}

	// Add the new state
	states.push_back(state);
	states.back()->init(window, &eventHandler);
}

void Game::popState()
{
	// Clean up the current state
	if ( !states.empty() ) {
		states.back()->reset();
		states.pop_back();
	}

	// Resume previous state
	if ( !states.empty() ) {
		states.back()->resume();
	}
}

void Game::run()
{
	// Switch to Level state
	changeState(Level::instance());

	sf::Event sf_event;
	Event event;

	while (running())
    {
		// Events
		handleEvents(event,sf_event);
		
		// Updates
		update();

		// Drawing
		draw();
    }

	// We left the game loop
	cleanStates();
}

void Game::quit() {
	is_running = false;
	window->Close();
}