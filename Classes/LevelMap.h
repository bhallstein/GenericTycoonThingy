/*
 * 
 *
 */

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Building.h"

using namespace std;


class LevelMap
{
public:
	LevelMap(int w, int h);
	~LevelMap();

	// Properties
	int Columns, Rows;
	vector<Building> Buildings;

	// Getters
	int numberOfBuildings();

	// Functions
	void Draw(sf::RenderWindow& window);

private:

};