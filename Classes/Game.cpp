#include "Game.hpp"

Game::Game(sf::RenderWindow *_window)
{
	int game_stage = DEMON_STAGE;	// Should be a setting or otherwise disk-saved (perhaps in its own file?)

	// Get game settings
	settings.load(false);	// false = don't load defaults
	
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
	int windowStyle = (settings.setMap["Windowed"].value == "1" ? sf::Style::Close : sf::Style::Fullscreen);
	std::string window_name = game_stage == TUTORIAL_STAGE ? "Happy Hair Tycoon" : "Demon Barber Tycoon";
 	window->Create(sf::VideoMode(800, 600), window_name, windowStyle);
	window->SetFramerateLimit(60);
}

Game::~Game()
{
	
}

void Game::Run()
{
	// Create Level
	Level level("Data/level1.xml", window, &eventHandler);
	
	bool should_quit = false;

	sf::Event sf_event;
	Event event;

	while (window->IsOpened() && !should_quit)
    {
		// Events
		event.loadFromMousePos(sf::Mouse::GetPosition(*window));					
		if (event.x > 0 && event.x < window->GetWidth() && event.y > 0 && event.y < window->GetHeight())
			eventHandler.dispatchEvent(&event);										
		
		while (window->PollEvent(sf_event)) {
			event.loadFromSFEvent(&sf_event);		// Convert sfml event to our own type
			
			// Close window : exit
		    if (event.type == CLOSED)
				window->Close();
			
			// Keys
			if (event.type == KEYPRESS) {
				if (event.key == K_ESC || event.key == K_Q)
					should_quit = true;
				else if (event.key == K_P)
					level.createPlaceable();
			}
			
			// Mouse events sent via eventhandler
			if (event.type == MOUSEMOVE) ;				// Ignore actual mouse moves
			eventHandler.dispatchEvent(&event);
		}
		
		level.updateObjects();
		level.destroyThings();	// Removed destroyed objects.
		
		// Drawing
		window->Clear(sf::Color(138,43,226));		// Electric Indigo, bitches
		level.draw();
        
        window->Display();	// Refresh the window
    }

	return;	// error?
}
