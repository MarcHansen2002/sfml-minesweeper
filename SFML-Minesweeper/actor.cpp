#include "actor.h"
#include "game.h"
#include <assert.h>
#include <iostream>

//Constructor
actor::actor()
{

}
//Initialise actor after the constructor
void actor::Init()
{
	LoadTexture(texturePath, texture);
	sprite.setTexture(texture);

	UpdateSprite();
	
}
void actor::Render(sf::RenderWindow& window)
{
	window.draw(sprite); //Draws actor's sprite
}
void actor::UpdateSprite()
{
	//Set sprite to a small section of the texture to allow for sprite sheets
	textRect = sf::IntRect(((sheetData.index - 1) % sheetData.columns) * texture.getSize().x / sheetData.columns, ((sheetData.index - 1) / sheetData.columns) * texture.getSize().y / sheetData.rows, texture.getSize().x / sheetData.columns, texture.getSize().y / sheetData.rows);
	//Set origin to the centre of the actor and set the sprite's texture
	sprite.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
	sprite.setTextureRect(textRect);

	posRect.left = location.x - textRect.width / 2.f;
	posRect.width = location.x + textRect.width / 2.f;
	posRect.top = location.y - textRect.height / 2.f;
	posRect.height = location.y + textRect.height / 2.f;
}
//When this actor is clicked
void actor::onLeftClick()
{
}
void actor::onRightClick()
{
}

void actor::CheckCollisions(sf::RenderWindow& window)
{
}
void actor::Update(float elapsed)
{
	//Update an actor's location and scale
	sprite.setPosition(location);
	sprite.setScale(scale);
}
void actor::DisplayHitbox(sf::RenderWindow& window)
{
	//Create a box to draw around an actor's hitbox
	sf::RectangleShape box;
	sf::FloatRect rect = GetRectCollision();
	box.setOrigin(0, 0);
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Red);
	box.setPosition(rect.left, rect.top);
	box.setSize({ (float)rect.width, (float)rect.height });
	box.setOutlineThickness(5);
	window.draw(box);

}
void actor::DisplayTextureRect(sf::RenderWindow& window)
{
	sf::RectangleShape box;
	sf::IntRect rect = this->sprite.getTextureRect();
	box.setOrigin(0, 0);
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Blue);
	box.setPosition(location.x, location.y);
	box.setSize({ (float)rect.width, (float)rect.height });
	box.setOutlineThickness(5);
	window.draw(box);
}


tile::tile()
{
	type = "tile";
	texturePath = "../Assets/SweeperSheet.png";
	scale = { 2, 2 };
	sheetData.columns = 3;
	sheetData.rows = 4;

	revealed = false;
	flagged = false;
}

void tile::Update(float elapsed)
{
	//Change the sprite used on the texture depending on the state of a tile
	actor::Update(elapsed);
	if (revealed)
	{
		sheetData.index = id; //Sets sprite to whatever the tile is (number, bomb, empty etc)
	}
	else if (flagged)
	{
		sheetData.index = 10; //Sets sprite to contain a flag to show a warning
	}
	else
	{
		sheetData.index = 12; //If tile is not revealed or flagged use the default sprite
	}
	UpdateSprite();

}
void tile::onLeftClick()
{
	//If first tile pressed is bomb, move bomb
	if (!gameInst->clickedAnywhere)
	{
		gameInst->clickedAnywhere = true;
		if (id == 11)
		{
			gameInst->MoveBomb(GridLoc);
		}
	}
	if (!flagged && !revealed)
	{
		revealed = true;
		if (id == 11)
		{
			//End Game, mine clicked
		}
		else if (id == 9)
		{
			//Empty tile, open surrounding tiles
			gameInst->OpenSurroundingEmptyTiles(gameInst->fieldSize, GridLoc);
		}
		else if (id <= 5)
		{
			//If tile has 5 or less bombs around check for empties. Impossible to have any empties if more than 5 bombs surround
			gameInst->CheckForEmpties(gameInst->fieldSize, GridLoc);
		}
	}
}
void tile::onRightClick()
{
	//Change a tile to be flagged if not flagged and vice versa if a tile is not revealed
	if (!revealed)
	{
		flagged = !flagged;
	}
}

sf::FloatRect actor::GetRectCollision()
{
	sf::IntRect TextRect = sprite.getTextureRect();
	sf::FloatRect PositionalRect = { location.x - ((TextRect.width / 2.f) * scale.x), //Left
		location.y - ((TextRect.height / 2.f) * scale.y), //Top
		((float)TextRect.width) * scale.x, //Right
		((float)TextRect.height) * scale.y }; //Bottom
	return PositionalRect;
}	
bool IsMouseColliding(actor& actor, sf::RenderWindow& window)
{
	sf::FloatRect actorPos = actor.GetRectCollision();
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	//Check mouse is not left of actor
	if (mousePos.x < actorPos.left)
	{
		return false;
	}
	//Check mouse is not right of actor
	if (mousePos.x > actorPos.width + actorPos.left)
	{
		return false;
	}
	//Check mouse is not above actor
	if (mousePos.y < actorPos.top)
	{
		return false;
	}
	//Check mouse is not below actor
	if (mousePos.y > actorPos.top + actorPos.height)
	{
		return false;
	}
	//If none of above is true, it is colliding
	return true;

}
bool LoadTexture(const sf::String& file, sf::Texture& texture)
{
	//Loads texture from specified file path
	if (texture.loadFromFile(file))
	{
		return true;
	}
	assert(false);
	return false;
}