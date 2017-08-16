/*
 * main.cpp – main game loop thinger
 *
 */

// #include <iostream>
#include <string>
// #include <map>
// #include <vector>

#include "Classes/Game.h"


int main(int argc, char *argv[], char *envp[])
{
	// Configuration: paths
	std::string path_settings = "Data/Settings.xml";
	// etc.
	
	// Start game
	Game g;
	g.Run();
}



// map<string,string> paraMap(int argc, char *argv[], char *envp[]) //consider validating the map's contents too - and removing invalid pairs (no need to error unnecessarily; just ignore)
// {
// 	map<string,string> theMap;
// 	string theKey;
// 	string theValue;
// 
// 	//first add argv[0] - always the executable path
// 	theKey = "ExePath";
// 	theValue = argv[0];
// 
// 	theMap[theKey] = theValue;
// 
// 	//command line switches (only check for switches (and then their values); we take no regular arguments)
// 	for (int i = 1;i < argc;i++)
// 	{
// 		//see if the parameter is a switch; ignore it if not
// 		if(argv[i][0] == '-') //check for the leading dash to identify
// 		{
// 			switch(argv[i][1])
// 			{
// 			case 'w':
// 				theKey = "Windowed";
// 				theValue = "1";
// 				break;
// 			case 'r':
// 				theKey = "Resolution";
// 				theValue = argv[++i]; //the parameter follows the switch
// 				break;
// 			default:
// 				//not accepted param
// 				theKey = "";
// 			}
// 
// 			if(theKey != "") //make sure the key isn't empty
// 			{
// 				//add it to the Map
// 				theMap[theKey] = theValue;
// 			}
// 		}
// 	}
// 
// 	//environment variables //later
// 	/*for (int n = 0;envp[n];n++) //while there are env items, iterate them
// 	{
// 
// 		cout << envp[n] << endl;
// 	}*/
// 
// 	return theMap;
// }