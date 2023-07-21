#pragma once
#include <SFML/Graphics.hpp>
//Sprite sheet data
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
	//Variables
	//References
	game *gameInst = nullptr; //Reference to the game object
	sf::RenderWindow* windowRef = nullptr; //Reference to the rendered window

	//Texture
	sf::IntRect textRect; //Displayed part of texture
	sf::IntRect posRect; //Rect of where actor is
	sf::Texture texture; //Loaded texture from texturePath
	sf::String texturePath; //Directory of texture
	sf::Sprite sprite;
	SpriteSheet sheetData;

	sf::Vector2f location;
	sf::Vector2f scale = { 1, 1 };
	sf::String type = "actor";

	//Functions
	actor();
	//Required
	void Init();
	virtual void Update(float elapsed);
	void UpdateSprite();
	virtual void Render(sf::RenderWindow& window);

	//Click Events
	virtual void OnLeftClick();
	virtual void OnRightClick();
	virtual void OnMiddleClick();

	//Collision
	void DisplayHitbox(sf::RenderWindow& window);
	void DisplayTextureRect(sf::RenderWindow& window);
	sf::FloatRect GetRectCollision();
};

class tile : public actor
{
public:
	//Variables
	bool revealed; //If the tile has been clicked (revealed)
	bool flagged; //If the tile has been flagged (right clicked)
	int id; //Type of tile it is: number, bomb, empty etc
	int gridLoc; //Location of tile on the grid

	//Functions
	tile();
	//Required
	void Update(float elapsed);

	//Click Events
	void OnLeftClick() override;
	void OnRightClick() override;
	void OnMiddleClick() override;
};

class playButton : public actor
{
public:
	//Variables
	sf::Vector2i size = { 10, 10 };
	int count = 10;

	//Functions
	playButton();
	void OnLeftClick();
};

class timer : public actor
{
public:
	//Variables
	sf::Font font;

	//Functions
	timer();
	void Render(sf::RenderWindow& window);
};
class flagCount : public actor
{
public:
	//Variables
	sf::Font font;

	//Functions
	flagCount();
	void Render(sf::RenderWindow& window);
};

bool LoadTexture(const sf::String& file, sf::Texture& texture);
bool IsMouseColliding(actor& actor, sf::RenderWindow& window);