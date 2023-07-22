#include "actor.h"
#include "game.h"
#include <assert.h>
#include <iostream>
#include <sstream>
#include <iomanip>

// ===================================================================== BASE ACTOR
actor::actor()
{

}
//Required
void actor::Init()
{
	windowRef = gameInst->windowRef;

	LoadTexture(texturePath, texture);
	sprite.setTexture(texture);

	UpdateSprite();
	
}
void actor::Render(sf::RenderWindow& window)
{
	window.draw(sprite); //Draws actor's sprite
}
void actor::Update(float elapsed)
{
	//Update an actor's location and scale
	sprite.setPosition(location);
	//Scale to a percent of screen
	if (stretchScale)
	{
		sf::Vector2f scaleSize = { (gameInst->resolution.x * scale.x) / textRect.width, (gameInst->resolution.y * scale.y) / textRect.height };
		sprite.setScale(scaleSize);
	}
	//Regularly scale off vector
	else
	{
		sprite.setScale(scale);
	}
	
}
void actor::UpdateSprite()
{
	//Set sprite to a small section of the texture to allow for sprite sheets
	textRect = sf::IntRect(((sheetData.index - 1) % sheetData.columns) * texture.getSize().x / sheetData.columns, ((sheetData.index - 1) / sheetData.columns) * texture.getSize().y / sheetData.rows, texture.getSize().x / sheetData.columns, texture.getSize().y / sheetData.rows);
	//Set origin to the centre of the actor and set the sprite's texture
	sprite.setOrigin((textRect.width / (1.f / origin.x)), (textRect.height / (1.f / origin.y)));
	sprite.setTextureRect(textRect);

	posRect.left = location.x - textRect.width / 2.f;
	posRect.width = location.x + textRect.width / 2.f;
	posRect.top = location.y - textRect.height / 2.f;
	posRect.height = location.y + textRect.height / 2.f;
}
//Click Events
void actor::OnLeftClick()
{
}
void actor::OnRightClick()
{
}
void actor::OnMiddleClick()
{

}

//Collision
sf::FloatRect actor::GetRectCollision()
{
	sf::IntRect TextRect = sprite.getTextureRect();
	float xScale = 1, yScale = 1;
	if (stretchScale)
	{
		xScale = (gameInst->resolution.x * scale.x) / textRect.width;
		yScale = (gameInst->resolution.y * scale.y) / textRect.height;
	}
	else
	{
		xScale = scale.x;
		yScale = scale.y;
	}

	sf::FloatRect PositionalRect;
	PositionalRect.left = location.x - ((TextRect.width / (1.f / origin.x)) * xScale);
	PositionalRect.top = location.y - ((TextRect.height / (1.f / origin.y)) * yScale);
	PositionalRect.width = ((float)TextRect.width) * xScale;
	PositionalRect.height = ((float)TextRect.height) * yScale;
	

	return PositionalRect;
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
//===================================================================== BASE ACTOR


//===================================================================== TILE
tile::tile()
{
	type = "tile";
	texturePath = "../Assets/SweeperSheet.png";
	scale = { 2, 2 };
	sheetData.columns = 3;
	sheetData.rows = 5;

	revealed = false;
	flagged = false;
}
//Required
void tile::Init()
{
	actor::Init();

	origin = { 0, 0 };
	//Get pixel size of one tile
	int sizeX, sizeY, i, x;
	sf::FloatRect size = GetRectCollision();
	sizeX = size.width - size.left;
	sizeY = size.height - size.top;

	//Get location of top left of where grid should be to be centered
	sf::Vector2f startLoc;
	startLoc.x = (gameInst->resolution.x / 2.f) - ((sizeX * gameInst->fieldSize.x) / 2.f);
	startLoc.y = (gameInst->resolution.y / 2.f) - ((sizeY * gameInst->fieldSize.y) / 2.f);

	//Set tile location within centered grid location
	i = gridLoc % gameInst->fieldSize.x;
	x = gridLoc / gameInst->fieldSize.x;
	location = { startLoc.x + (i * (textRect.width * scale.x)), startLoc.y + (x * (textRect.height * scale.y)) };
	
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
//Click Events
void tile::OnLeftClick()
{
	if (!gameInst->canClick)
	{
		//Cancels event if canClick is false
		return;
	}
	//If first tile pressed is bomb, move bomb
	if (!gameInst->clickedAnywhere)
	{
		gameInst->clickedAnywhere = true;
		gameInst->AddMines(gridLoc);
		gameInst->SetNumbers();
	}
	//Logic for any click after starting
	if (!flagged && !revealed)
	{
		revealed = true;
		//Mine pressed
		if (id == 11)
		{
			//Set tile to clicked mine sprite
			id = 15;
			//End game with a loss
			gameInst->GameOver();
			return;
		}
		//Empty pressed
		else if (id == 9)
		{
			//Empty tile, open surrounding tiles
			gameInst->OpenSurroundingTiles(gridLoc);
		}
		//Number under 6 pressed
		else if (id <= 5)
		{
			//If tile has 5 or less bombs around check for empties. Impossible to have any empties if more than 5 bombs surround
			gameInst->OpenSurroundingEmpties(gridLoc);
		}
		gameInst->tilesToWin--;
		
		//Last tile pressed
		if (gameInst->tilesToWin <= 0)
		{
			gameInst->GameWin();
		}
	}
}
void tile::OnRightClick()
{
	//Cancel event if canClick is false
	if (!gameInst->canClick)
	{
		return;
	}
	//Change a tile to be flagged if not flagged and vice versa if a tile is not revealed
	if (!revealed)
	{
		if (flagged)
		{
			gameInst->remainingFlags++;
		}
		else
		{
			gameInst->remainingFlags--;
		}
		flagged = !flagged;
	}
}
void tile::OnMiddleClick()
{
	//Cancel event if canClick is false
	if (!gameInst->canClick)
	{
		return;
	}
	if (revealed)
	{
		//Get all tiles and sort them
		std::vector<tile*> tiles = gameInst->GetAllTiles(true);
		//Get all tiles surrounding clicked tile
		std::vector<tile*> surrounding;
		int flaggedAmount = 0;
		if (!gameInst->TileOnTop(gridLoc))
		{
			surrounding.push_back(tiles[gridLoc - (gameInst->fieldSize.x)]);
			if (!gameInst->TileOnLeft(gridLoc))
			{
				surrounding.push_back(tiles[gridLoc - (gameInst->fieldSize.x + 1)]);
			}
			if (!gameInst->TileOnRight(gridLoc))
			{
				surrounding.push_back(tiles[gridLoc - (gameInst->fieldSize.x - 1)]);
			}
		}
		if (!gameInst->TileOnLeft(gridLoc))
		{
			surrounding.push_back(tiles[gridLoc - 1]);
		}
		if (!gameInst->TileOnRight(gridLoc))
		{
			surrounding.push_back(tiles[gridLoc + 1]);
		}
		if (!gameInst->TileOnBot(gridLoc))
		{
			surrounding.push_back(tiles[gridLoc + (gameInst->fieldSize.x)]);
			if (!gameInst->TileOnLeft(gridLoc))
			{
				surrounding.push_back(tiles[gridLoc + (gameInst->fieldSize.x - 1)]);
			}
			if (!gameInst->TileOnRight(gridLoc))
			{
				surrounding.push_back(tiles[gridLoc + (gameInst->fieldSize.x + 1)]);
			}
		}
		//Check the amount of flagged tiles around clicked tile
		for (int i = 0; i < surrounding.size(); i++)
		{
			if (surrounding[i]->flagged)
			{
				flaggedAmount++;
			}
		}
		//If amount of touching flags = id then open all surrounding tiles
		if (flaggedAmount == id)
		{
			//Loop through surrounding tiles
			for (int i = 0; i < surrounding.size(); i++)
			{
				surrounding[i]->OnLeftClick();
			}
		}
	}
}
//===================================================================== TILE


//===================================================================== PLAY BUTTON
playButton::playButton()
{
	type = "button";
	texturePath = "../Assets/Button.png";
}
//Click Events
void playButton::OnLeftClick()
{
	gameInst->PlayGame(size, count);
}
//===================================================================== PLAY BUTTON


//===================================================================== TIMER & FLAG COUNT
timer::timer()
{
	type = "ui";
	texturePath = "../Assets/TimeDisplay.png";
	if (!font.loadFromFile("../Assets/Fonts/arial.ttf"))
	{
		assert(false);
	}
}
void timer::Render(sf::RenderWindow& window)
{
	window.draw(sprite);
	sf::Text text;

	std::stringstream stream;
	float time = gameInst->time;
	stream << std::fixed << std::setprecision(2) << time;

	text.setString(stream.str());
	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setCharacterSize(24);
	text.setOrigin(textRect.width / 2, textRect.height / 2);
	text.setPosition(location.x - 40, location.y-5);
	window.draw(text);
}

flagCount::flagCount()
{
	type = "ui";
	texturePath = "../Assets/FlagDisplay.png";
	if (!font.loadFromFile("../Assets/Fonts/arial.ttf"))
	{
		assert(false);
	}
}
void flagCount::Render(sf::RenderWindow& window)
{
	window.draw(sprite);
	sf::Text text;

	text.setString(std::to_string(gameInst->remainingFlags));
	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setCharacterSize(24);
	text.setOrigin(textRect.width / 2, textRect.height / 2);
	text.setPosition(location.x - 40, location.y-5);
	window.draw(text);
}
//===================================================================== TIMER & FLAG COUNT


background::background(sf::String TextPath)
{
	texturePath = TextPath;
	origin = { 0, 0 };
	stretchScale = true;
}

bool IsMouseColliding(actor& actor, sf::RenderWindow& window)
{
	sf::FloatRect actorPos = actor.GetRectCollision();
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
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