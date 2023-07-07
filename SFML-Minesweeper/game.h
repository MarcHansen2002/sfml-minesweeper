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

	void Update(sf::RenderWindow& window, float elapsed);
	void Render(sf::RenderWindow& window, float elapsed);
	void Init(sf::RenderWindow& window);
	game();
};

