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
	sf::RenderWindow* windowRef = nullptr;
	sf::IntRect textRect;
	sf::IntRect posRect;
	sf::Texture texture;
	sf::String texturePath;
	sf::Sprite sprite;
	SpriteSheet sheetData;
	sf::Vector2f location;
	sf::Vector2f scale;

	actor();
	void Init();
	void Render(sf::RenderWindow& window);
	virtual void onLeftClick();
	virtual void onRightClick();
	void CheckCollisions(sf::RenderWindow& window);
	virtual void Update(float elapsed);
	void DisplayHitbox(sf::RenderWindow& window);
	void DisplayTextureRect(sf::RenderWindow& window);
	void UpdateSprite();
	sf::FloatRect GetRectCollision();
};

class tile : public actor
{
public:
	bool revealed; //If the tile has been clicked (revealed)
	bool flagged; //If the tile has been flagged (right clicked)
	int id; //Type of tile it is: number, bomb, empty

	tile();
	void Update(float elapsed);
	void onLeftClick();
	void onRightClick() override;
};

bool LoadTexture(const sf::String& file, sf::Texture& texture);
bool IsMouseColliding(actor& actor, sf::RenderWindow& window);