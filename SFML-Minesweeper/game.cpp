#include "game.h"

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
	//The mapsize of the minesweeper grid
	sf::Vector2i mapSize = { 10, 10 };
	//Loop through X and Y of the grid and instantiate a tile for each location
	for (int i = 0; i < mapSize.x; i++)
	{
		for (int x = 0; x < mapSize.y; x++)
		{
			//Create a new tile and put it on the heap
			tile* tileObj = nullptr;
			tileObj = new tile;

			//Add the tile to the actors list and initialise it
			tileObj->id = 1;//(i * mapSize.y) + x;
			actors.push_back(tileObj);
			int act = actors.size() - 1;
			actors[act]->Init();
			actors[act]->location = { 100 + (i * (actors[act]->textRect.width * actors[act]->scale.x)), 100 + (x * (actors[act]->textRect.height * actors[act]->scale.y)) };
		}
	}
}