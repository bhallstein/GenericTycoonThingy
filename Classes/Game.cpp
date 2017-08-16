#include "Game.h"

Game::Game()
{
	/* Window setup */

	// Set defaults (manually for now)
	w = 50; h = 40;
	block_w = 16; block_h = 15;
	std::string window_name = "Demon Barber Tycoon";
	
	// Get settings from file and if success & values valid, replace defaults.
	Settings.Load(false); //false = don't load defaults

	int windowStyle;
	if(Settings.SetMap["Windowed"].Value == "1")
		windowStyle = sf::Style::Close;
	else windowStyle = sf::Style::Fullscreen;
	
 	DBTWindow.Create(sf::VideoMode(w * block_w, h * block_h), window_name, windowStyle);
	DBTWindow.SetFramerateLimit(60);
}

Game::~Game()
{
	// Destroy things - incl. DBTWindow, I guess.
}

void Game::Run()
{
	// Create Level
	Level level(w, h);
	
	int pixel_w = w * block_w;
	int pixel_h = h * block_h;
	
	bool should_quit = false;
	EventResponder *privileged_event_responder = NULL; 		// All P.E.R.s must be subclasses of EventResponder, & implement 
															// the receiveEvent(sf::Event *ev, ER *p_e_r) method
	sf::Event sf_event;
	Event event;
    
	while (DBTWindow.IsOpened() && !should_quit)
    {
		/* Events */

        while (DBTWindow.PollEvent(sf_event))
		{
			// Convert sf event to our own type
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
						std::cout << "creating new building... ";
						privileged_event_responder = (EventResponder*) level.createBuilding();
					}
				}
				// Mouse
				else if (event.type == MOUSEMOVE) // or click! 
				{
					// Send to relevant thing(s), use memory mapping
					// get maploc for mouse location
					// send event to each’s receiveEvent() method
				}
			}
        }
    

		/* Drawing */
		
		DBTWindow.Clear(sf::Color(138,43,226));		// Electric Indigo

		level.draw(DBTWindow, block_w, block_h);

        // Update the window
        DBTWindow.Display();
    }

	return;	// return error
}

// 
// bool Game::Action(char Input[256], Builder& bill)
// {
// 		bool GameOver = false;
// 		Input = strcat(Input, " default");	//error c2440 cannot convert from char* to char[256]
// 		char *first = strtok(Input, " ");
// 		char *second = strtok(NULL, " ");
// 		if(second != 0)
// 		{
// 			string uno = static_cast<string>(first);
// 			string dos = static_cast<string>(second);
// 		
// 			if(uno == "build")
// 			{
// 				bill.addBuilding(dos, m_Money);
// 			}
// 			else if(uno == "help")
// 			{
// 				Tutorial tut;
// 				tut.Info();
// 				cout << "\nClosing Tutorial\n";
// 			}	
// 			else if(uno == "report")
// 			{
// 				bill.BuildingReport();
// 			}
// 			else if(uno == "quit")
// 			{
// 				cout << "\nEnding Game.";
// 				GameOver = true;
// 			}
// 			else if(uno == "skip")
// 			{
// 				cout << "No action being taken this day.\n\n";
// 				bill.m_action = true;
// 			}
// 			else
// 			{
// 				bool change = false;
// 				for(int i = 0; i < bill.m_structName.size(); i++)
// 				{
// 					if(uno == bill.m_structName[i])//match structname
// 					{
// 						if(dos == bill.m_Action1[i])//first action - sets action to true
// 						{					
// 							Structure* moo = bill.m_pBuildFarLeft;
// 							while(change == false && moo != 0)
// 							{
// 								if((moo->GetType() == bill.m_structName[i]) && (moo->GetService() == false))
// 								{
// 									moo->SetService();
// 									change = true;
// 									cout << endl << bill.m_structName[i] << " set to " << bill.m_Action1[i];
// 									cout << "\n\n";
// 								}
// 								moo = moo->GetRight();
// 							}
// 					}
// 					else if(dos == bill.m_Action2[i])//second action - sets action to false
// 						{
// 							Structure* moo = bill.m_pBuildFarLeft;
// 							while(change == false && moo != 0)
// 							{
// 								if((moo->GetType() == bill.m_structName[i]) && (moo->GetService() == true))
// 								{
// 									moo->SetService();
// 									change = true;
// 									cout << endl << bill.m_structName[i] << " set to " << bill.m_Action2[i];
// 									cout << "\n\n";
// 								}
// 								moo = moo->GetRight();
// 							}
// 						}
// 						else
// 						{
// 							cout << "\nError: Unrecognised action for " << bill.m_structName[i] <<"s!\n\n";
// 						}
// 					}
// 				}
// 				if(!change)
// 				{
// 					cout << "\nUnrecognised command. Please try again.\n\n";
// 					
// 				}
// 			}		
// 		}
// 		return GameOver;
// }
