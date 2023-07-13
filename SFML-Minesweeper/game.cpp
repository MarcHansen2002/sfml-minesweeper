#include "game.h"

void game::Update(sf::RenderWindow& window, float elapsed)
{
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->CheckCollisions(window);
		actors[i]->Update(elapsed);
	}
}
void game::Render(sf::RenderWindow& window, float elapsed)
{
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->Render(window);
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
	sf::Vector2i mapSize = { 10, 10 };
	for (int i = 0; i < mapSize.x; i++)
	{
		for (int x = 0; x < mapSize.y; x++)
		{
			tile* tileObj = nullptr;
			tileObj = new tile;

			tileObj->id = 1;//(i * mapSize.y) + x;
			actors.push_back(tileObj);
			int act = actors.size() - 1;
			actors[act]->Init();
			actors[act]->location = { 100 + (i * (actors[act]->textRect.width * actors[act]->scale.x)), 100 + (x * (actors[act]->textRect.height * actors[act]->scale.y)) };
		}
	}
}