#pragma once
#include <SFML/Graphics.hpp>
#include "actor.h"

class game
{
public:
	bool DebugMode;
	std::vector<actor*> actors;
	sf::RenderWindow* windowRef;

	tile TestTile;

	void Update(sf::RenderWindow& window, float elapsed); //Called every frame to update actors
	void Render(sf::RenderWindow& window, float elapsed); //Called every frame to draw all actors
	void Init(sf::RenderWindow& window); //Initialise the main game
	void ClearActors(); //Clears all the actors and removes them from the heap
	void GenerateField(sf::Vector2i mapSize);
	void AddMines(int amount);
	game();
};

