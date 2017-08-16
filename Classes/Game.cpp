#include "Game.hpp"

Game::Game()
{
	/* Window setup */

	// Set defaults (manually for now)
	block_w = 16; block_h = 16;	// Can calculate these otherwise: pixel_w/w and pixel_h/h
	std::string window_name = "Demon Barber Tycoon";
	
	// Get settings from file and if success & values valid, replace defaults.
	Settings.Load(false); //false = don't load defaults

	int windowStyle;
	if(Settings.SetMap["Windowed"].Value == "1")
		windowStyle = sf::Style::Close;
	else windowStyle = sf::Style::Fullscreen;
	
 	DBTWindow.Create(sf::VideoMode(50 * block_w, 40 * block_h), window_name, windowStyle);
	DBTWindow.SetFramerateLimit(60);
}

Game::~Game()
{
	// Destroy things - incl. DBTWindow, I guess.
}

void Game::Run()
{
	// Create Level
	Level level("Data/level1.xml", &gamemap);
	
	bool should_quit = false;
	EventResponder *privileged_event_responder = NULL; 		// All P.E.R.s must be subclasses of EventResponder, & implement 
															// the receiveEvent(sf::Event *ev) method
	int pixel_w = w * block_w;
	int pixel_h = h * block_h;
	
	sf::Event sf_event;
	Event event;
    
	while (DBTWindow.IsOpened() && !should_quit)
    {
		// Events

        while (DBTWindow.PollEvent(sf_event))
		{
			// Convert sfml event to our own type
			event.loadFromSFEvent(&sf_event, block_w, block_h);
			
			// Close window : exit
            if (event.type == CLOSED)
	   			DBTWindow.Close();
			
			// If there is privileged event responder: send events to it
			if (privileged_event_responder != NULL) {
				// The game loop can't always know what the P.E.R. should do with the event
				// 	– it depends on what the P.E.R. is up to. We merely send the event on.
				EventResponder *resp = (EventResponder*) privileged_event_responder;
				resp->receiveEvent(&event, &privileged_event_responder);
			}
			// Eventually, we'll send events either via the memory map, or to the game UI.
			else {
				// Keys
				if (event.type == KEYPRESS) {
					if (event.key == K_ESC || event.key == K_Q) {
						should_quit = true;
					}
					else if (event.key == K_B) {
						privileged_event_responder = (EventResponder*) level.createPlaceable();
					}
				}
				// Mouse
				else if (event.type == MOUSEMOVE || event.type == LEFTCLICK) {
					// Send to relevant thing(s), use memory mapping
					gamemap.dispatchEvent(&event, &privileged_event_responder);
				}
			}
        }
		
		// Drawing
		
		DBTWindow.Clear(sf::Color(138,43,226));		// Electric Indigo, bitches
		level.draw(DBTWindow, block_w, block_h);

        // Update the window
        DBTWindow.Display();
    }

	return;	// return error?
}
