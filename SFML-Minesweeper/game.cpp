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
		if (DebugMode)
		{
			actors[i]->DisplayHitbox(*windowRef);
		}
	}
}
game::game()
{
	DebugMode = false;
}
void game::Init(sf::RenderWindow& window)
{
	//Clear all actors and remove them from heap
	ClearActors();
	//Adds tiles to scene
	GenerateField({ 10, 10 });
	AddMines(10);
	SetNumbers({ 10, 10 });
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
}

void game::GenerateField(sf::Vector2i fieldSize)
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
			tileObj->id = 1;
			tileObj->Init();
			tileObj->location = { 100 + (i * (tileObj->textRect.width * tileObj->scale.x)), 100 + (x * (tileObj->textRect.height * tileObj->scale.y)) };
			actors.push_back(tileObj);
		}
	}
}
void game::AddMines(int amount)
{
	//Get all the tile actors in a vector
	std::vector<actor*> tiles;
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->type == "tile")
		{
			tiles.push_back(actors[i]);
		}
	}
	//Randomly assign bomb ids to some of those tiles
	for (int i = 0; i < amount; i++)
	{
		int selected = rand() % (tiles.size() - 1);
		dynamic_cast<tile*>(tiles[selected])->id = 11;
		tiles.erase(tiles.begin() + selected);
	}
}
void game::SetNumbers(sf::Vector2i fieldSize)
{
	//Get all the tile actors in a vector
	std::vector<actor*> tiles;
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->type == "tile")
		{
			tiles.push_back(actors[i]);
		}
	}
	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), sortTiles);

	//Loop through them to set what number they will be
	for (int i = 0; i < tiles.size(); i++)
	{
		if (dynamic_cast<tile*>(tiles[i])->id != 11)
		{
			int surrounding = 0;
			bool onLeft = (i % fieldSize.x == 0); //Checks if tile is on the far left of the field
			bool onRight = (i % fieldSize.x == fieldSize.x - 1); //Checks if tile is on the far right of the field
			bool onTop = (i / fieldSize.y == 0); // Checks if the tile is on the top of the field
			bool onBot = (i / fieldSize.y == fieldSize.y - 1); //Checks if the tile is on the bottom of the field

			if (!onTop)
			{
				//Check above tile
				if (dynamic_cast<tile*>(tiles[i - fieldSize.x])->id == 11) { surrounding++; }
				//Check above left tile
				if (!onLeft)
				{
					if (dynamic_cast<tile*>(tiles[i - (fieldSize.x + 1)])->id == 11) { surrounding++; }
				}
				//Check above right tile
				if (!onRight)
				{
					if (dynamic_cast<tile*>(tiles[i - (fieldSize.x - 1)])->id == 11) { surrounding++; }
				}
			}
			if (!onLeft)
			{
				//Check left tile
				if (dynamic_cast<tile*>(tiles[i - 1])->id == 11) { surrounding++; }
			}
			if (!onRight)
			{
				//Check right tile
				if (dynamic_cast<tile*>(tiles[i + 1])->id == 11) { surrounding++; }
			}
			if (!onBot)
			{
				//Check below tile
				if (dynamic_cast<tile*>(tiles[i + fieldSize.x])->id == 11) { surrounding++; }
				//Check below left tile
				if (!onLeft)
				{
					if (dynamic_cast<tile*>(tiles[i + (fieldSize.x - 1)])->id == 11) { surrounding++; }
				}
				//Check below right tile
				if (!onRight)
				{
					if (dynamic_cast<tile*>(tiles[i + (fieldSize.x + 1)])->id == 11) { surrounding++; }
				}
			}
			if (surrounding > 0)
			{
				dynamic_cast<tile*>(tiles[i])->id = 10 - surrounding;
			}
		}
	}
}