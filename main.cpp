#include <map>
#include "game.h"
#include <SFML\Graphics.hpp>

using namespace sf;
using namespace std;

// O hai everyone

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

int main(int argc, char *argv[], char *envp[])
{
	/*int error = 0; // used to return an error code, or 0 for no error

	map<string,string> argMap;
	argMap = paraMap(argc,argv,envp);

	//iterate through the map, outputting params!
	for(map<string,string>::iterator iter = argMap.begin(); iter != argMap.end(); iter++)
	{
		cout << iter->first << " = " << iter->second << endl; //output key = value
	}

	cin.get();

	cout << "-----------Welcome to Demon Barber Tycoon!------------\n\n\n";
	cout << "Starting new game. Select 'Help' for instructions.\n\n";
	
	Game dbt;
	dbt.Run();

	return error;*/
//SFML STUFF
	// Create the main window
    RenderWindow window(VideoMode(800, 600), "SFML window");

	// Load a sprite to display
     Texture texture;
     texture.LoadFromFile("cute_image.png");
     Sprite sprite(texture);

	// Start the game loop
     while (window.IsOpened())
     {
         // Process events
         Event event;
         while (window.PollEvent(event))
         {
             // Close window : exit
             if (event.Type == Event::Closed)
                 window.Close();
         }
 
         // Clear screen
         window.Clear(Color(100,149,237));

		 window.Draw(sprite);
 
         // Update the window
         window.Display();
     }
}



/*
//SFML STUFF
#include <SFML\Graphics.hpp>

using namespace sf;

int main()
{
	// Create the main window
    RenderWindow window(VideoMode(800, 600), "SFML window");

	// Load a sprite to display
     Texture texture;
     texture.LoadFromFile("cute_image.png");
     Sprite sprite(texture);

	// Start the game loop
     while (window.IsOpened())
     {
         // Process events
         Event event;
         while (window.PollEvent(event))
         {
             // Close window : exit
             if (event.Type == Event::Closed)
                 window.Close();
         }
 
         // Clear screen
         window.Clear(Color(100,149,237));

		 window.Draw(sprite);
 
         // Update the window
         window.Display();
     }
}*/