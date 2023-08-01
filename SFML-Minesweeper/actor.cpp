#include "actor.h"
#include "game.h"

// ===================================================================== BASE ACTOR
actor::actor()
{
	texture = nullptr;
}

//Required
void actor::Init()
{
	windowRef = gameInst->windowRef;

	texture = gameInst->resources.GetTexture(texturePath);
	sprite.setTexture(*texture);

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
	//Scale the sprite
	sf::Vector2f scaleSize = GetScale();
	sprite.setScale(scaleSize);
}
void actor::UpdateSprite()
{
	//Set sprite to a small section of the texture to allow for sprite sheets
	textRect = sf::IntRect(((sheetData.index - 1) % sheetData.columns) * texture->getSize().x / sheetData.columns, ((sheetData.index - 1) / sheetData.columns) * texture->getSize().y / sheetData.rows, texture->getSize().x / sheetData.columns, texture->getSize().y / sheetData.rows);
	//Set origin to the centre of the actor and set the sprite's texture
	sprite.setOrigin((textRect.width / (1.f / origin.x)), (textRect.height / (1.f / origin.y)));
	sprite.setTextureRect(textRect);
}

sf::Vector2f actor::GetScale()
{
	sf::Vector2f scaleSize = { 1, 1 };
	switch (scaleType)
	{
	case normal:
		scaleSize = scale;
		break;
	case stretch:
		scaleSize = { (gameInst->resolution.x * scale.x) / textRect.width, (gameInst->resolution.y * scale.y) / textRect.height };
		break;
	case pixel:
		scaleSize;
		//Returns a default value if both values are 0
		if ((scale.x <= 0) && (scale.y <= 0))
		{
			scaleSize = { 1, 1 };
		}
		else
		{
			if (scale.x <= 0)
			{
				scaleSize.x = (scale.y / (float)textRect.height);
				scaleSize.y = (scale.y / (float)textRect.height);
			}
			else if (scale.y <= 0)
			{
				scaleSize.x = (scale.x / (float)textRect.width);
				scaleSize.y = (scale.x / (float)textRect.width);
			}
			else
			{
				scaleSize.x = (scale.x / (float)textRect.width);
				scaleSize.y = (scale.y / (float)textRect.height);
			}
		}
		break;
	default:
		scaleSize = scale;
		break;
	}
	return scaleSize;
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
	sf::Vector2f scaleVal = GetScale();
	sf::FloatRect PositionalRect;

	PositionalRect.left = location.x - ((TextRect.width / (1.f / origin.x)) * scaleVal.x);
	PositionalRect.top = location.y - ((TextRect.height / (1.f / origin.y)) * scaleVal.y);
	PositionalRect.width = ((float)TextRect.width) * scaleVal.x;
	PositionalRect.height = ((float)TextRect.height) * scaleVal.y;
	
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
	texturePath = "SweeperSheet.png";
	scale = { 2.8, 2.8 };
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
	float sizeX, sizeY, i, x;
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
	location = { startLoc.x + (i * (textRect.width * scale.x)), 58 + startLoc.y + (x * (textRect.height * scale.y)) };
	
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
		//Adds 1 to current attempts of this difficulty
		gameInst->metrics.currentData.attempts++;
	}
	//Logic for any click after starting
	if (!flagged && !revealed)
	{
		revealed = true;
		//Mine pressed
		if (id == 11)
		{
			gameInst->resources.PlaySound("Sounds/Explosion.wav");
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
		gameInst->resources.PlaySound("Sounds/TileOpen.wav");
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


//===================================================================== TIMER & FLAG COUNT
timer::timer()
{
	type = "ui";
	texturePath = "TimeDisplay.png";
	if (!font.loadFromFile("Assets/Fonts/arial.ttf"))
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
	texturePath = "FlagDisplay.png";
	if (!font.loadFromFile("Assets/Fonts/arial.ttf"))
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


image::image(sf::String TextPath)
{
	texturePath = TextPath;
	origin = { 0.5f, 0.5f };
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
	//If no texture path is specified, do not try to load
	if ((file.isEmpty()) || (file == ""))
	{
		return false;
	}
	//Loads texture from specified file path
	if (texture.loadFromFile(file))
	{
		return true;
	}
	assert(false);
	return false;
}

//===================================================================== BUTTONS
button::button()
{
	type = "button";
	texturePath = "Button.png";
	origin = { 0.5, 0.5 };
}

void button::Render(sf::RenderWindow& window)
{
	if (useSprite)
	{
		window.draw(sprite);
	}
	else
	{
		sf::RectangleShape buttonShape;
		if (!hovered)
		{
			buttonShape.setFillColor(boxColour);
			buttonShape.setOutlineColor(borderColour);
			buttonShape.setOutlineThickness(borderSize);
			buttonShape.setSize(boxSize);
		}
		else
		{
			buttonShape.setFillColor(hoveredBoxColour);
			buttonShape.setOutlineColor(hoveredBorderColour);
			buttonShape.setOutlineThickness(hoveredBorderSize);
			buttonShape.setSize(hoveredBoxSize);
		}

		
		buttonShape.setOrigin(buttonShape.getLocalBounds().width / 2.f, buttonShape.getLocalBounds().height / 2.f);
		buttonShape.setPosition(location);
		windowRef->draw(buttonShape);
	}

	//Render text
	sf::Text textOBJ;

	textOBJ.setString(text);
	textOBJ.setFont(*gameInst->resources.GetFont("Assets/Fonts/arial.ttf"));
	textOBJ.setFillColor(sf::Color::Black);
	textOBJ.setCharacterSize(24);
	
	sf::FloatRect objRect = textOBJ.getLocalBounds();
	textOBJ.setOrigin(objRect.width / 2.f, objRect.height / 2.f);
	textOBJ.setPosition(location.x -5, location.y -5);

	window.draw(textOBJ);
}
void button::OnLeftClick()
{
	gameInst->resources.PlaySound("Sounds/Click.wav");
}

sf::FloatRect button::GetRectCollision()
{
	sf::FloatRect TextRect;
	if (useSprite)
	{
		TextRect = (sf::FloatRect)sprite.getTextureRect();
	}
	else
	{
		sf::RectangleShape shape;
		shape.setSize(boxSize);
		shape.setOutlineThickness(borderSize);

		TextRect = shape.getLocalBounds();
	}
	sf::Vector2f scaleVal = GetScale();
	scaleVal = { 1, 1 };
	sf::FloatRect PositionalRect;
	PositionalRect.left = location.x - ((TextRect.width / (1.f / origin.x)) * scaleVal.x);
	PositionalRect.top = location.y - ((TextRect.height / (1.f / origin.y)) * scaleVal.y);
	PositionalRect.width = ((float)TextRect.width) * scaleVal.x;
	PositionalRect.height = ((float)TextRect.height) * scaleVal.y;
	return PositionalRect;

}


//Play Button
playButton::playButton()
{
	scale = { 4, 2 };
}
void playButton::OnLeftClick()
{
	button::OnLeftClick();
	gameInst->metrics.currentData.difficultyName = difficulty;

	gameInst->PlayGame(size, count);
}

//Replay Button
retryButton::retryButton()
{
	scale = { 4, 2 };
	text = "Retry";
}
void retryButton::OnLeftClick()
{
	button::OnLeftClick();
	gameInst->Init(*windowRef);
}
//Menu Button
menuButton::menuButton()
{
	text = "Menu";
	scale = { 4, 2 };
}
void menuButton::OnLeftClick()
{
	button::OnLeftClick();
	gameInst->state = gameInst->GameState::menu;
	gameInst->Init(*windowRef);
}
//Exit Button
exitButton::exitButton()
{
	text = "Exit";
	scale = { 4, 2 };
}
void exitButton::OnLeftClick()
{
	button::OnLeftClick();
	windowRef->close();
}
//Stats Button
statsButton::statsButton()
{
	text = "Stats";
}
void statsButton::OnLeftClick()
{
	button::OnLeftClick();
	gameInst->state = gameInst->GameState::stats;
	gameInst->Init(*windowRef);
}
//Help Button
helpButton::helpButton()
{
	text = "Help";
}
void helpButton::OnLeftClick()
{
	button::OnLeftClick();
	gameInst->state = gameInst->GameState::help;
	gameInst->Init(*windowRef);
}


//===================================================================== BUTTONS
textBox::textBox()
{
	if (!font.loadFromFile("Assets/Fonts/arial.ttf"))
	{
		assert(false);
	}
	text.setString(string);
	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(textSize);
}

void textBox::Render(sf::RenderWindow& window)
{
	text.setString(string);
	text.setFont(font);
	text.setFillColor(textColour);
	text.setCharacterSize(textSize);
	sf::FloatRect objRect = text.getLocalBounds();
	text.setOrigin(objRect.width / (1 / origin.x), objRect.height / (1/origin.y));
	text.setPosition(location.x, location.y - 5);

	window.draw(text);
}