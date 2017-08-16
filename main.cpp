/*
 * main.cpp – main game loop thinger
 *
 */

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/* Game classes */
// #include "Classes/Address.h"
// #include "Classes/Builder.h"
// #include "Classes/Building.h"
// #include "Classes/Game.h"
// #include "Classes/Level.h"
// #include "Classes/SettingsManager.h"
// #include "Classes/Structure.h"
// #include "Classes/Tutorial.h"

// using namespace std;
using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

using std::string;
using std::vector;
using std::map;


int main(int argc, char *argv[], char *envp[])
{
	// Configuration: paths
	string path_settings = "Data/Settings.xml";
	// etc.

	/* Window setup */

	// Set defaults (manually for now)
	int w_width = 800, w_height = 600;
	string w_name = "Demon Barber Tycoon";
	
	// Get settings from file and if success & values valid, replace defaults.
	//...
	
	sf::RenderWindow DBT_Window(sf::VideoMode(w_width, w_height), w_name, sf::Style::Close);

	
	// Arrays for units, buildings
	
	

	bool should_quit = false;
    
	while (DBT_Window.IsOpened() && !should_quit)
    {
        sf::Event ev;
        while (DBT_Window.PollEvent(ev)) 
		{
            // Close window : exit
            if (ev.Type == sf::Event::Closed)
	   		DBT_Window.Close();
	
			// Mouseovers: pass to underlying objects
			// Current affairs will need to overrule: say you are dragging a member of staff across the map.
			// How to do this unclear atm.
			
			if (ev.Type == sf::Event::KeyPressed) {
				sf::Keyboard::Key keycode = ev.Key.Code;
				if (keycode == sf::Keyboard::Escape)
					should_quit = true;
				if (keycode == 'b')
					; // activate barbershop building
				
			}
        }
    
        DBT_Window.Clear(sf::Color::Black);
    
	    //window.Draw(sprite);
    
        // Update the window
        DBT_Window.Display();
    }
}



map<string,string> paraMap(int argc, char *argv[], char *envp[]) //consider validating the map's contents too - and removing invalid pairs (no need to error unnecessarily; just ignore)
{
	map<string,string> theMap;
	string theKey;
	string theValue;

	//first add argv[0] - always the executable path
	theKey = "ExePath";
	theValue = argv[0];

	theMap[theKey] = theValue;

	//command line switches (only check for switches (and then their values); we take no regular arguments)
	for (int i = 1;i < argc;i++)
	{
		//see if the parameter is a switch; ignore it if not
		if(argv[i][0] == '-') //check for the leading dash to identify
		{
			switch(argv[i][1])
			{
			case 'w':
				theKey = "Windowed";
				theValue = "1";
				break;
			case 'r':
				theKey = "Resolution";
				theValue = argv[++i]; //the parameter follows the switch
				break;
			default:
				//not accepted param
				theKey = "";
			}

			if(theKey != "") //make sure the key isn't empty
			{
				//add it to the Map
				theMap[theKey] = theValue;
			}
		}
	}

	//environment variables //later
	/*for (int n = 0;envp[n];n++) //while there are env items, iterate them
	{

		cout << envp[n] << endl;
	}*/

	return theMap;
}