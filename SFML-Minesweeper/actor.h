#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <assert.h>


enum ScaleMode
{
	normal, //Scales by a vector multiplied
	stretch, //Scales to a percent of the resolution
	pixel //Scales to an amount of pixels. If x OR y is 0 or less they will be scaled equally with the non-0 axis
};
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
	
	//Control
	bool hovered = false;

	//References
	game *gameInst = nullptr; //Reference to the game object
	sf::RenderWindow* windowRef = nullptr; //Reference to the rendered window
	int zLayer = 1;

	//Texture
	sf::IntRect textRect; //Displayed part of texture
	sf::Texture* texture; //Loaded texture from texturePath
	sf::String texturePath; //Directory of texture

	sf::Sprite sprite;
	SpriteSheet sheetData;

	sf::Vector2f location;
	sf::Vector2f origin = { 0.5, 0.5 };
	ScaleMode scaleType = normal;
	sf::Vector2f scale = { 1, 1 }; //Change to vector2i and have bool for stretch scale. Stretch scale scales to % of screen, false is regular
	sf::String type = "actor";

	//Functions
	actor();
	//Required
	virtual void Init();
	virtual void Update(float elapsed);
	void UpdateSprite();
	virtual void Render(sf::RenderWindow& window);


	sf::Vector2f GetScale();
	//Click Events
	virtual void OnLeftClick();
	virtual void OnRightClick();
	virtual void OnMiddleClick();

	//Collision
	void DisplayHitbox(sf::RenderWindow& window);
	void DisplayTextureRect(sf::RenderWindow& window);
	virtual sf::FloatRect GetRectCollision();
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
	void Init() override;
	void Update(float elapsed);

	//Click Events
	void OnLeftClick() override;
	void OnRightClick() override;
	void OnMiddleClick() override;
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

class image : public actor
{
public:
	//Function
	image(sf::String TextPath);
};

//===================================================================== BUTTONS
class button : public actor
{
public:
	//Variables
	sf::Font font;
	sf::String text;

	//Button styling
	bool useSprite = false;
	//Regular
	sf::Vector2f boxSize = { 150,50 };
	float borderSize = 5;
	sf::Color boxColour = sf::Color(82, 82, 82);
	sf::Color borderColour = sf::Color(28, 28, 28);
	//Hovered
	sf::Vector2f hoveredBoxSize = { 165, 55 };
	float hoveredBorderSize = 10;
	sf::Color hoveredBoxColour = sf::Color(138, 138, 138);
	sf::Color hoveredBorderColour = sf::Color(59, 59, 59);

	//Functions
	button();
	void Render(sf::RenderWindow& window);
	void OnLeftClick();

	sf::FloatRect GetRectCollision() override;
};

class playButton : public button
{
public:
	//Variables
	std::string difficulty;

	sf::Vector2i size = { 10, 10 };
	int count = 10;

	//Functions
	playButton();
	void OnLeftClick();
};
class retryButton : public button
{
public:
	retryButton();
	void OnLeftClick();
};
class menuButton : public button
{
public:
	menuButton();
	void OnLeftClick();
};
class exitButton : public button
{
public:
	exitButton();
	void OnLeftClick();
};
class helpButton : public button
{
public:
	helpButton();
	void OnLeftClick();
};
class statsButton : public button
{
public:
	statsButton();
	void OnLeftClick();
};
//===================================================================== BUTTONS

class textBox : public actor
{
public:
	//Variables
	sf::Text text;
	sf::Font font;
	sf::String string;

	sf::Vector2f origin = { 0.5f, 0.5f };

	int textSize = 24;
	sf::Color textColour = sf::Color::White;
	//Functions
	textBox();
	void Render(sf::RenderWindow& window);
};


bool LoadTexture(const sf::String& file, sf::Texture& texture);
bool IsMouseColliding(actor& actor, sf::RenderWindow& window);