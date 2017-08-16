/*
 * Game.h - Game class, the main class for running the game, really.
 *
 */

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


class Game
{
public:
	Game();
	~Game();

	// Instance methods
	void Run();

private:
	
	// Instance Variables
	sf::RenderWindow DBTWindow;
	
};
