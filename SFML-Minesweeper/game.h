#pragma once
#include <SFML/Graphics.hpp>
#include "actor.h"
enum GameState
{
	menu,
	play
};
class game
{
public:
	GameState state = GameState::menu;
	bool DebugMode;
	std::vector<actor*> actors;
	sf::RenderWindow* windowRef;

	sf::Vector2i fieldSize = { 30, 16 };
	int mineCount = 99;
	bool clickedAnywhere = false;

	void Update(sf::RenderWindow& window, float elapsed); //Called every frame to update actors
	void Render(sf::RenderWindow& window, float elapsed); //Called every frame to draw all actors
	void Init(sf::RenderWindow& window); //Initialise the main game
	void ClearActors(); //Clears all the actors and removes them from the heap
	void GenerateField(sf::Vector2i mapSize);
	void AddMines(int amount);
	void SetNumbers(sf::Vector2i fieldSize);
	void OpenSurroundingEmptyTiles(sf::Vector2i fieldSize, int tileLoc);
	void CheckForEmpties(sf::Vector2i fieldSize, int tileLoc);
	void ForceOpenTile(sf::Vector2i fieldSize, int tileLoc, std::vector<tile*> tiles);
	void SoftForceOpenTile(sf::Vector2i fieldSize, int tileLoc, std::vector<tile*> tiles);
	void MoveBomb(int tileLoc);
	void PlayGame(sf::Vector2i fieldSize, int mineCount);
	std::vector<tile*> getAllTiles();
	game();

	void InitGame();
	void InitMenu();
};

