#include "game.h"
#include <iostream>
bool SortTiles(actor* a, actor* b)
{
	//Return A if A is above B
	if (a->location.y != b->location.y)
	{
		return (a->location.y < b->location.y);
	}
	//Return A if A AND B are on the same layer but A is further left than B
	else
	{
		return (a->location.x < b->location.x);
	}
}
//Required
game::game()
{
	windowRef = nullptr;
	debugMode = false;
}
void game::Init(sf::RenderWindow& window)
{
	switch (state)
	{
	case GameState::menu:
		InitMenu();
		break;
	case GameState::play:
		InitGame();
		break;
	default:
		break;
	}
}
void game::Update(sf::RenderWindow& window, float elapsed)
{
	//Loop through actor list, check collisions and update
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->Update(elapsed);
	}
	for (int i = 0; i < uiActors.size(); i++)
	{
		uiActors[i]->Update(elapsed);
	}
	if (clickedAnywhere)
	{
		time += elapsed;
	}
}
void game::Render(sf::RenderWindow& window, float elapsed)
{
	//Loop through all actors and draw them to window
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->Render(window);
		//If debug mode is enabled this will draw hitboxes around each actor
		if (debugMode)
		{
			actors[i]->DisplayHitbox(*windowRef);
		}
	}
	for (int i = 0; i < uiActors.size(); i++)
	{
		uiActors[i]->Render(window);
	}
}

void game::ClearActors()
{
	//Loops through all actors
	for (int i = 0; i < actors.size(); i++)
	{
		//Delete actor from this location off the heap
		delete actors[i];
		//Clear the reference to deleted actor
		actors[i] = nullptr;
	}
	actors.clear();
}

//Minesweeper Related
void game::GenerateField() //Creates a grid of tiles size depending on fieldSize variable
{
	//Loop through X and Y of the grid and instantiate a tile for each location
	for (int i = 0; i < fieldSize.x; i++)
	{
		for (int x = 0; x < fieldSize.y; x++)
		{
			//Create a new tile and put it on the heap
			tile* tileObj = nullptr;
			tileObj = new tile;

			//Add the tile to the actors list and initialise it
			tileObj->gridLoc = ((1 + i) + (fieldSize.x * x)) - 1;
			tileObj->gameInst = this;
			tileObj->Init();
			tileObj->location = { 100 + (i * (tileObj->textRect.width * tileObj->scale.x)), 100 + (x * (tileObj->textRect.height * tileObj->scale.y)) };
			actors.push_back(tileObj);
		}
	}
}
void game::AddMines(int gridLoc) //Add mines randomly throughout the grid but not in a 3*3 area around first click
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles = GetAllTiles(true);
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(gridLoc, tiles);

	//Remove clicked tile from list
	tiles[gridLoc] = nullptr;
	//Loop through surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		//Remove surrounding tiles from list
		tiles[surrounding[i]->gridLoc] = nullptr;
	}
	//Set the amount of tiles removed to size of surrounding + 1
	int removable = surrounding.size() + 1;
	//Loop through list of tiles
	for (int i = 0; i < tiles.size(); i++)
	{
		//If tile has been removed take it out the list
		if (tiles[i] == nullptr)
		{
			tiles.erase(tiles.begin() + i);
			i--;
			removable--;
			if (removable <= 0)
			{
				i = tiles.size() + 1;
			}
		}
	}
	//Randomly assign bomb ids to some of the remaining tiles
	for (int i = 0; i < mineCount; i++)
	{
		int selected = rand() % (tiles.size() - 1);
		tiles[selected]->id = 11;
		tiles.erase(tiles.begin() + selected);
	}
}
void game::SetNumbers() //Calculate how many bombs are around each tile and set IDs accordingly
{
	std::vector<tile*> tiles = GetAllTiles();

	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), SortTiles);

	//Loop through them to set what number they will be
	for (int i = 0; i < tiles.size(); i++)
	{
		if (tiles[i]->id != 11)
		{
			int surroundingBombs = 0;
			//Get all surrounding tiles around current tile
			std::vector<tile*> surroundingTiles = GetSurroundingTiles(i, tiles);
			//Loop through surrounding tiles
			for (int x = 0; x < surroundingTiles.size(); x++)
			{
				//If surrounding tile is a bomb, add 1 to the surrounding bombs
				if (surroundingTiles[x]->id == 11)
				{
					surroundingBombs++;
				}
			}
			//Changes id depending on how many bombs surround the tile
			if (surroundingBombs > 0)
			{
				tiles[i]->id = surroundingBombs; //ID for numbered tile depending on amount of bombs surrounding
			}
			else
			{
				tiles[i]->id = 9; //ID for empty tile
			}
		}
	}
}
void game::OpenSurroundingEmpties(int tileLoc) //Open all surrounding tiles if they're empty
{
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(tileLoc);
	//Loop through all surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		if (surrounding[i]->id == 9)
		{
			//Open tile if it is empty
			surrounding[i]->OnLeftClick();
		}
	}
}
void game::OpenSurroundingTiles(int tileLoc) //Open all surrounding tiles no matter what
{
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(tileLoc);
	//Loop through all surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		//Open surrounding tiles
		surrounding[i]->OnLeftClick();
	}
}
void game::PlayGame(sf::Vector2i size, int count)
{
	fieldSize = size;
	mineCount = count;

	tilesToWin = (fieldSize.x * fieldSize.y) - mineCount;
	time = 0.f;
	remainingFlags = mineCount;

	state = play;
	Init(*windowRef);
}
std::vector<tile*> game::GetAllTiles(bool sort)
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles;
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->type == "tile")
		{
			tiles.push_back(dynamic_cast<tile*>(actors[i]));
		}
	}

	if (sort)
	{
		std::sort(tiles.begin(), tiles.end(), SortTiles);
	}
	return tiles;
}
std::vector<tile*> game::GetSurroundingTiles(int tileLoc) //Gets all surrounding tiles
{
	std::vector<tile*> tiles = GetAllTiles(true);
	std::vector<tile*> surrounding;
	if (!TileOnTop(tileLoc)) //Check above
	{
		surrounding.push_back(tiles[tileLoc - fieldSize.x]);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x + 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x - 1)]);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		surrounding.push_back(tiles[tileLoc - 1]);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		surrounding.push_back(tiles[tileLoc + 1]);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		surrounding.push_back(tiles[tileLoc + fieldSize.x]);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x - 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x + 1)]);
		}
	}

	return surrounding;
}
std::vector<tile*> game::GetSurroundingTiles(int tileLoc, std::vector<tile*> tiles) //Gets all surrounding tiles but doesn't fetch each tile everytime
{
	std::vector<tile*> surrounding;
	if (!TileOnTop(tileLoc)) //Check above
	{
		surrounding.push_back(tiles[tileLoc - fieldSize.x]);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x + 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x - 1)]);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		surrounding.push_back(tiles[tileLoc - 1]);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		surrounding.push_back(tiles[tileLoc + 1]);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		surrounding.push_back(tiles[tileLoc + fieldSize.x]);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x - 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x + 1)]);
		}
	}

	return surrounding;
}
//Tile Location Checking
bool game::TileOnBot(int tileLoc)
{
	return (tileLoc / fieldSize.x == fieldSize.y - 1);
}
bool game::TileOnLeft(int tileLoc)
{
	return (tileLoc % fieldSize.x == 0);
}
bool game::TileOnTop(int tileLoc)
{
	return (tileLoc / fieldSize.x == 0);
}
bool game::TileOnRight(int tileLoc)
{
	return (tileLoc % fieldSize.x == fieldSize.x - 1);
}
//Game Ending
void game::GameOver()
{
	canClick = false;
	clickedAnywhere = false;

	std::vector<tile*> tiles = GetAllTiles();
	for (int i = 0; i < tiles.size(); i++)
	{
		//Correctly flagged bomb
		if ((tiles[i]->id == 11) && (tiles[i]->flagged))
		{
			tiles[i]->id = 14;
			tiles[i]->revealed = true;
		}
		//Incorrectly flagged bomb
		else if ((tiles[i]->id != 11) && (tiles[i]->flagged))
		{
			tiles[i]->id = 13;
			tiles[i]->revealed = true;
		}
		//Revealed bomb
		else if ((tiles[i]->id == 11) && (!tiles[i]->revealed))
		{
			tiles[i]->revealed = true;
		}
	}
}
void game::GameWin()
{
	canClick = false;
	clickedAnywhere = false;
}
//Scene Initialising
void game::InitGame()
{
	//Clear all actors and remove them from heap
	ClearActors();

	//Adds tiles to scene
	GenerateField();

	timer* gameTimer = nullptr;
	gameTimer = new timer;
	gameTimer->location = { 600, 50 };
	gameTimer->scale = { 3, 3 };
	gameTimer->gameInst = this;
	gameTimer->Init();
	uiActors.push_back(gameTimer);

	flagCount* flagCounter = nullptr;
	flagCounter = new flagCount;
	flagCounter->location = { 400, 50 };
	flagCounter->scale = { 3, 3 };
	flagCounter->gameInst = this;
	flagCounter->Init();
	uiActors.push_back(flagCounter);
}
void game::InitMenu()
{
	ClearActors();

	playButton* easyButton = nullptr;
	easyButton = new playButton;
	easyButton->location = { windowRef->getSize().x / 2.f, 500 };
	easyButton->scale = { 3, 3 };
	easyButton->size = { 9, 9 };
	easyButton->count = 10;
	easyButton->gameInst = this;
	easyButton->Init();

	playButton* normalButton = nullptr;
	normalButton = new playButton;
	normalButton->location = { windowRef->getSize().x / 2.f, 600 };
	normalButton->scale = { 3, 3 };
	normalButton->size = { 16, 16 };
	normalButton->count = 40;
	normalButton->gameInst = this;
	normalButton->Init();

	playButton* hardButton = nullptr;
	hardButton = new playButton;
	hardButton->location = { windowRef->getSize().x / 2.f, 700 };
	hardButton->scale = { 3, 3 };
	hardButton->size = { 30, 16 };
	hardButton->count = 99;
	hardButton->gameInst = this;
	hardButton->Init();

	actors.push_back(easyButton);
	actors.push_back(normalButton);
	actors.push_back(hardButton);
}