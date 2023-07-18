#include "game.h"
#include <iostream>

bool sortTiles(actor* a, actor* b)
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
std::vector<tile*> game::getAllTiles()
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
	return tiles;
}

void game::Update(sf::RenderWindow& window, float elapsed)
{
	//Loop through actor list, check collisions and update
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->CheckCollisions(window);
		actors[i]->Update(elapsed);
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
}
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

void game::GenerateField()
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
void game::AddMines(int gridLoc)
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles = getAllTiles();
	std::sort(tiles.begin(), tiles.end(), sortTiles);

	int removable = 1;
	//Remove clicked tile from spawns
	tiles[gridLoc] = nullptr;
	
	if (!TileOnTop(gridLoc))
	{
		//Remove tile above clicked from spawns
		tiles[gridLoc - fieldSize.x] = nullptr;
		removable++;
		if (!TileOnLeft(gridLoc))
		{
			tiles[gridLoc - (fieldSize.x + 1)] = nullptr;
			removable++;
		}
		if (!TileOnRight(gridLoc))
		{
			tiles[gridLoc - (fieldSize.x - 1)] = nullptr;
			removable++;
		}
	}
	if (!TileOnLeft(gridLoc))
	{
		tiles[gridLoc - 1] = nullptr;
		removable++;
	}
	if (!TileOnRight(gridLoc))
	{
		tiles[gridLoc + 1] = nullptr;
		removable++;
	}
	if (!TileOnBot(gridLoc))
	{
		tiles[gridLoc + fieldSize.x] = nullptr;
		removable++;
		if (!TileOnLeft(gridLoc))
		{
			tiles[gridLoc + (fieldSize.x - 1)] = nullptr;
			removable++;
		}
		if (!TileOnRight(gridLoc))
		{
			tiles[gridLoc + (fieldSize.x + 1)] = nullptr;
			removable++;
		}
	}
	for (int i = 0; i < tiles.size(); i++)
	{
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
	//Randomly assign bomb ids to some of those tiles
	for (int i = 0; i < mineCount; i++)
	{
		int selected = rand() % (tiles.size() - 1);
		tiles[selected]->id = 11;
		tiles.erase(tiles.begin() + selected);
	}
}
void game::SetNumbers()
{
	std::vector<tile*> tiles = getAllTiles();

	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), sortTiles);

	//Loop through them to set what number they will be
	for (int i = 0; i < tiles.size(); i++)
	{
		if (tiles[i]->id != 11)
		{
			int surrounding = 0;
			if (!TileOnTop(i))
			{
				//Check above tile
				if (tiles[i - fieldSize.x]->id == 11) { surrounding++; }
				//Check above left tile
				if (!TileOnLeft(i))
				{
					if (tiles[i - (fieldSize.x + 1)]->id == 11) { surrounding++; }
				}
				//Check above right tile
				if (!TileOnRight(i))
				{
					if (tiles[i - (fieldSize.x - 1)]->id == 11) { surrounding++; }
				}
			}
			if (!TileOnLeft(i))
			{
				//Check left tile
				if (tiles[i - 1]->id == 11) { surrounding++; }
			}
			if (!TileOnRight(i))
			{
				//Check right tile
				if (tiles[i + 1]->id == 11) { surrounding++; }
			}
			if (!TileOnBot(i))
			{
				//Check below tile
				if (tiles[i + fieldSize.x]->id == 11) { surrounding++; }
				//Check below left tile
				if (!TileOnLeft(i))
				{
					if (tiles[i + (fieldSize.x - 1)]->id == 11) { surrounding++; }
				}
				//Check below right tile
				if (!TileOnRight(i))
				{
					if (tiles[i + (fieldSize.x + 1)]->id == 11) { surrounding++; }
				}
			}
			//Changes id depending on how many bombs surround the tile
			if (surrounding > 0)
			{
				tiles[i]->id = surrounding; //ID for numbered tile depending on amount of bombs surrounding
			}
			else
			{
				tiles[i]->id = 9; //ID for empty tile
			}
		}
	}
}
void game::CheckForEmpties(int tileLoc)
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles = getAllTiles();
	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), sortTiles);

	if (!TileOnTop(tileLoc)) //Check above
	{
		SoftForceOpenTile((tileLoc - fieldSize.x), tiles);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			SoftForceOpenTile((tileLoc - (fieldSize.x + 1)), tiles);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			SoftForceOpenTile((tileLoc - (fieldSize.x - 1)), tiles);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		SoftForceOpenTile((tileLoc - 1), tiles);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		SoftForceOpenTile((tileLoc + 1), tiles);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		SoftForceOpenTile((tileLoc + fieldSize.x), tiles);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			SoftForceOpenTile((tileLoc + (fieldSize.x - 1)), tiles);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			SoftForceOpenTile((tileLoc + (fieldSize.x + 1)), tiles);
		}
	}
}
void game::OpenSurroundingEmptyTiles(int tileLoc)
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles = getAllTiles();
	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), sortTiles);
	
	if (!TileOnTop(tileLoc)) //Check above
	{
		ForceOpenTile((tileLoc - fieldSize.x), tiles);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			ForceOpenTile((tileLoc - (fieldSize.x + 1)), tiles);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			ForceOpenTile((tileLoc - (fieldSize.x - 1)), tiles);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		ForceOpenTile((tileLoc - 1), tiles);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		ForceOpenTile((tileLoc + 1), tiles);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		ForceOpenTile((tileLoc + fieldSize.x), tiles);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			ForceOpenTile((tileLoc + (fieldSize.x - 1)), tiles);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			ForceOpenTile((tileLoc + (fieldSize.x + 1)), tiles);
		}
	}
}
void game::ForceOpenTile(int tileLoc, std::vector<tile*> tiles)
{
	bool previouslyFound = tiles[tileLoc]->revealed;
	tiles[tileLoc]->revealed = true;
	if ((tiles[tileLoc]->id == 9) && (!previouslyFound))
	{
		OpenSurroundingEmptyTiles(tileLoc);
	}
}
void game::SoftForceOpenTile(int tileLoc, std::vector<tile*> tiles)
{
	if (tiles[tileLoc]->id == 9)
	{
		bool previouslyFound = tiles[tileLoc]->revealed;
		tiles[tileLoc]->revealed = true;
		if (!previouslyFound)
		{
			OpenSurroundingEmptyTiles(tileLoc);
		}
	}
}
void game::MoveBomb(int tileLoc)
{
	std::vector<tile*> tiles = getAllTiles();
	std::sort(tiles.begin(), tiles.end(), sortTiles);

	tiles[tileLoc]->id = 1;
	for (int i = 0; i < tiles.size(); i++)
	{
		if ((i != tileLoc) && (tiles[tileLoc]->id != 11))
		{
			tiles[i]->id = 11;
			i = tiles.size() + 1;
		}
	}
	SetNumbers();
}
void game::PlayGame(sf::Vector2i size, int count)
{
	fieldSize = size;
	mineCount = count;

	state = play;
	Init(*windowRef);
}

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

void game::InitGame()
{
	//Clear all actors and remove them from heap
	ClearActors();
	//Adds tiles to scene
	GenerateField();
}
void game::InitMenu()
{
	ClearActors();

	playButton* easyButton = nullptr;
	easyButton = new playButton;
	easyButton->location = { windowRef->getSize().x / 2.f, 500};
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