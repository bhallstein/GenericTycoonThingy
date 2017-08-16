/*
 * main.cpp - the magic begins here
 *
 */

#include <string>
#include <SFML/Window.hpp>

#include "Classes/Game.hpp"

int main(int argc, char *argv[], char *envp[])
{
	srand(time(0));
	
	// Create game window
	sf::RenderWindow dbt_window;

	// Start game
	Game g(&dbt_window);
	g.Run();
}
