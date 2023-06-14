#include "game.h"

void game::Update(sf::RenderWindow& window, float elapsed)
{

}
void game::Render(sf::RenderWindow& window, float elapsed)
{
	for (int i = 0; i < actors.size(); i++)
	{
		window.draw(actors[i]->sprite);
	}
}
game::game()
{
}
void game::Init(sf::RenderWindow& window)
{
	
}