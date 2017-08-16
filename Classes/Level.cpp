#include <sstream>
#include "fstream"
#include "Level.h"
Level::Level(Builder& bill)//maybe add string, to load up fixed level - string could be provided by a file which man reads
{
	//read data from a file, create appropriate level
	std::string output;
	std::ifstream myfile ("Levels_Data.txt");
	if (myfile.is_open())
  {
    while (! myfile.eof() )//eof = end of file
    {
		getline (myfile,output);
		std::string buf; // Have a buffer string
		std::stringstream ss(output); // Insert the string into a stream
		std::vector<std::string> tokens;
		while (ss >> buf)
		{
			tokens.push_back(buf);//can be redone - see below. Take first word, then keep the rest (allow for multi word add)
		}
		std::string x = tokens[1];
		bill.addAddress(x);
    }
    myfile.close();
  }
	else
	{
		std::cout << "Unable to open file";
	}
	
	//load up all building data
	std::ifstream BuildingInfo ("Building_Data2.txt");
	if(BuildingInfo.is_open())
	{
		std::vector<std::string> tokens;
		while(! BuildingInfo.eof())
		{
			getline(BuildingInfo, output);
			tokens.push_back(output);			
		}
		bill.LoadStructures(tokens);
	}

}

Level::~Level(void)
{
	//bill.clear function - delete it all
}

void Level::eventCheck(int day)
{
	//as days pass, fixed things happen in a level - should also be read from a file...
}

