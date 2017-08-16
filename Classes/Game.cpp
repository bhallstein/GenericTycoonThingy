#include "Game.h"

//#include "Level.h"
#include "Building.h"


Game::Game()
{
	/* Window setup */

	// Set defaults (manually for now)
	pixel_width = 800, pixel_height = 600;
	std::string w_name = "Demon Barber Tycoon";
	
	// Get settings from file and if success & values valid, replace defaults.
	// ...
	
 	DBTWindow.Create(sf::VideoMode(pixel_width, pixel_height), w_name, sf::Style::Close);
	DBTWindow.SetFramerateLimit(60);
}

Game::~Game()
{
	// Destroy things - incl. DBTWindow, I guess.
}

void Game::Run()
{
	// Create Level
	int default_map_columns = 100, default_map_rows = 100;
	Level level(default_map_columns, default_map_rows, pixel_width, pixel_height);
	
	bool should_quit = false;
	EventResponder *privileged_event_responder = NULL; 		// All P.E.R.s must be subclasses of EventResponder, & implement 
															// the receiveEvent(sf::Event *ev, ER *p_e_r) method
	sf::Event ev;
    
	while (DBTWindow.IsOpened() && !should_quit)
    {
		/* Events */

        while (DBTWindow.PollEvent(ev)) 
		{
            // Close window : exit
            if (ev.Type == sf::Event::Closed)
	   			DBTWindow.Close();
	
			// If there is privileged event responder: send events to it
			if (privileged_event_responder != NULL) {
				// The game loop can't always know what the P.E.R. should do with the event
				// 	– it depends on what the P.E.R. is up to. We merely send the event on.
				EventResponder *resp = (EventResponder*) privileged_event_responder;
				resp->receiveEvent(&ev, &privileged_event_responder);
			}
			else {
				// Keys
				if (ev.Type == sf::Event::KeyPressed) {
					sf::Keyboard::Key keycode = ev.Key.Code;
				
					if (keycode == sf::Keyboard::Escape || keycode == sf::Keyboard::Q) {
						should_quit = true;
					}
					else if (keycode == sf::Keyboard::B) {
						std::cout << "creating new building... ";
						Building *b = level.createBuilding();
						privileged_event_responder = (EventResponder*) b;	// not sure if cast to EvResp needed here

						std::cout << "adding building to memory map... ";
					}
				}
				// Mouse
				else if (ev.Type == sf::Event::MouseMoved) 
				{
					// Send to relevant things, use memory mapping
					// get maploc for mouse location
					// send event to each’s receiveEvent() method
				}
			}
        }
    

		/* Drawing */
		
		DBTWindow.Clear(sf::Color(138,43,226));		// Electric Indigo

		level.draw(DBTWindow);

        // Update the window
        DBTWindow.Display();
    }
	
	// Builder bill;
	// 	Level level1(bill); //eventually need level manager to load certain levels
	// 	int error = 0;
	// 	bool GameOver = false;
	// 	do
	// 	{
	// 		bill.actionFalse();
	// 		bill.setMoney(m_Money);
	// 
	// 		cout << "Day " << m_Days << endl;
	// 		cout << "Money: " << m_Money << endl;
	// 		cout << "Please Enter an action.\n";
	// 		
	// 		char Input[256];
	// 		cin.getline ( Input, 256, '\n' );
	// 
	// 		if(Input != "")
	// 		{
	// 			GameOver = Action(Input, bill);	
	// 		}
	// 		else
	// 		{
	// 			cout << "No command has been entered - please enter an action.\n\n";
	// 		}
	// 		
	// 			
	// 		bool action = bill.returnAction();
	// 		if(action)
	// 		{
	// 			m_Money = bill.getMoney();
	// 			GatherIncome(bill);
	// 			m_Days++;
	// 		}
	// 	}
	// 	while(!GameOver);

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
// 
// void Game::GatherIncome(Builder& bill)
// {
// 	Structure* moo = bill.m_pBuildFarLeft;
// 	while (moo != 0)
// 	{
// 		m_Money += moo->GetIncome();
// 		moo = moo->GetRight();
// 	}
// }


