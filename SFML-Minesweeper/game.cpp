#include "game.h"
#include <iostream>

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
			tileObj->id = 2;
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