/*
 * main.cpp - the magic begins here
 *
 */

#include <string>
#include <SFML/Window.hpp>

#include "Classes/Game.hpp"
#include "Classes/SettingsManager.hpp"

int main(int argc, char *argv[], char *envp[])
{
	srand(time(0));
	
	// Create SettingsManager
	SettingsManager dbt_settings(argv);

	// Create game window
	sf::RenderWindow dbt_window;

	// Start game
	Game g(&dbt_window, &dbt_settings);
	g.run();
}
