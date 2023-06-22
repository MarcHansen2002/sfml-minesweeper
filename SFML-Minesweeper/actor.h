#pragma once
#include <SFML/Graphics.hpp>
struct SpriteSheet
{
	int index = 1;
	int rows = 1;
	int columns = 1;
};

class actor
{
public:
	sf::IntRect textRect;
	sf::Texture texture;
	sf::String texturePath;
	sf::Sprite sprite;
	SpriteSheet sheetData;
	sf::Vector2f location;
	sf::Vector2f scale;

	actor();
	void Init();
	void onClick();
	void Update(float elapsed);
};

class tile : public actor
{
public:
	bool revealed;
	int id;

	tile();
	void Update(float elapsed);
};

bool LoadTexture(const sf::String& file, sf::Texture& texture);

