#pragma once
#include <SFML/Graphics.hpp>
struct SpriteSheet
{
	int index = 1;
	int rows = 1;
	int columns = 1;
};
class game;

class actor
{
public:
	game *gameInst = nullptr;
	sf::RenderWindow* windowRef = nullptr;
	sf::IntRect textRect;
	sf::IntRect posRect;
	sf::Texture texture;
	sf::String texturePath;
	sf::Sprite sprite;
	SpriteSheet sheetData;
	sf::Vector2f location;
	sf::Vector2f scale = { 1, 1 };
	sf::String type = "actor";

	actor();
	void Init();
	virtual void Render(sf::RenderWindow& window);
	virtual void OnLeftClick();
	virtual void OnRightClick();
	virtual void OnMiddleClick();
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
	int gridLoc; //Location of tile on the grid

	tile();
	void Update(float elapsed);
	void OnLeftClick() override;
	void OnRightClick() override;
	void OnMiddleClick() override;
};

class playButton : public actor
{
public:
	playButton();
	sf::Vector2i size = { 10, 10 };
	int count = 10;

	void OnLeftClick();
};

class timer : public actor
{
public:
	sf::Font font;
	timer();
	void Render(sf::RenderWindow& window);
};
class flagCount : public actor
{
public:
	sf::Font font;

	flagCount();
	void Render(sf::RenderWindow& window);
};


bool LoadTexture(const sf::String& file, sf::Texture& texture);
bool IsMouseColliding(actor& actor, sf::RenderWindow& window);