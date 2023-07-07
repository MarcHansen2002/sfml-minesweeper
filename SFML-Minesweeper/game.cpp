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
		window.draw(actors[i]->sprite);
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
	tile Test;
	Test.windowRef = windowRef;
	Test.Init();
	TestTile.Init();
	actors.push_back(&TestTile);
}