#pragma once
#include <SFML/Graphics.hpp>
class actor
{
public:
	sf::Sprite sprite;
	sf::Vector2f location;
	sf::Vector2f scale;

	actor();
	void onClick();
	void Update(float elapsed);
};

