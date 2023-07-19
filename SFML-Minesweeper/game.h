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
	bool debugMode;
	std::vector<actor*> actors; //Objects within the game
	std::vector<actor*> uiActors; //Same as actors however not checked for collisions
	sf::RenderWindow* windowRef = nullptr;

	int tilesToWin;
	float time;
	int remainingFlags;
	sf::Vector2i fieldSize = { 30, 16 };
	int mineCount = 99;
	bool clickedAnywhere = false;

	void Update(sf::RenderWindow& window, float elapsed); //Called every frame to update actors
	void Render(sf::RenderWindow& window, float elapsed); //Called every frame to draw all actors
	void Init(sf::RenderWindow& window); //Initialise the main game
	void ClearActors(); //Clears all the actors and removes them from the heap
	void GenerateField();
	void AddMines(int gridLoc);
	void SetNumbers();
	void OpenSurroundingEmptyTiles(int tileLoc);
	void CheckForEmpties(int tileLoc);
	void ForceOpenTile(int tileLoc, std::vector<tile*> tiles);
	void SoftForceOpenTile(int tileLoc, std::vector<tile*> tiles);
	void MoveBomb(int tileLoc);
	void PlayGame(sf::Vector2i fieldSize, int mineCount);

	void GameWin();
	void GameOver();

	bool TileOnLeft(int tileLoc);
	bool TileOnTop(int tileLoc);
	bool TileOnBot(int tileLoc);
	bool TileOnRight(int tileLoc);
	std::vector<tile*> getAllTiles(bool sort = false);
	game();

	void InitGame();
	void InitMenu();
};

