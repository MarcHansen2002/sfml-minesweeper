#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "actor.h"
#include "database.h"



class Metrics
{
public:
	const std::string VERSION = "1.0";

	bool useDB = true;
	database db;

	struct difficultyData
	{
		std::string difficultyName;
		float time;
		int attempts;
		int completions;
	};
	difficultyData currentData;
	std::vector<difficultyData> diffData;
	std::string filePath;

	//Resets current stored stats
	//void Restart();

	bool Load(const std::string& path)
	{
		return DBLoad(path);
	}
	bool Save(const std::string& path = "")
	{
		return DBSave(path);
	}

	bool DBSave(const std::string& path = "");
	bool DBLoad(const std::string& path = "");

	void GetCurrentData();
	void StoreCurrentData();
	difficultyData GetDifficultyData(std::string diff);
};

class game
{
public:
	enum GameState
	{
		menu,
		play,
		stats,
		help
	};
	//Variables
	GameState state = GameState::menu;
	bool debugMode;
	sf::Vector2f resolution;

	//References
	Metrics metrics;
	sf::RenderWindow* windowRef = nullptr;
	
	//Actors
	std::vector<actor*> actors; //Objects within the game
	std::vector<actor*> uiActors; //Same as actors however not checked for collisions
	std::vector<actor*> allActors; //List of all actors combined to draw in order of zlayer
	
	//Minesweeper Related
	int tilesToWin;
	float time;
	int remainingFlags;
	sf::Vector2i fieldSize = { 30, 16 };
	int mineCount = 99;
	bool clickedAnywhere = false;
	bool canClick = true;

	//Functions
	game();
	//Required
	void Update(sf::RenderWindow& window, float elapsed); //Called every frame to update actors
	void Render(sf::RenderWindow& window, float elapsed); //Called every frame to draw all actors
	void Init(sf::RenderWindow& window); //Initialise the main game
	void CheckMouseCollisions(); //Checks if any collisions happen between actors and the mouse for hover events

	//Actor Handling
	void AddActor(actor* actor, bool collidable = true); //Inits and adds an actor to vectors
	void ClearActors(); //Clears all the actors and removes them from the heap

	//Minesweeper related
	void GenerateField();
	void AddMines(int gridLoc);
	void SetNumbers();
	void OpenSurroundingTiles(int tileLoc);
	void OpenSurroundingEmpties(int tileLoc);
	void PlayGame(sf::Vector2i fieldSize, int mineCount);
	std::vector<tile*> GetSurroundingTiles(int tileLoc);
	std::vector<tile*> GetSurroundingTiles(int tileLoc, std::vector<tile*> tiles);
	//Tile Location Checking
	bool TileOnLeft(int tileLoc);
	bool TileOnTop(int tileLoc);
	bool TileOnBot(int tileLoc);
	bool TileOnRight(int tileLoc);
	//Game Ending
	void GameWin();
	void GameOver();
	void AddFinishButtons();

	std::vector<tile*> GetAllTiles(bool sort = false);

	//Initialise a scene
	void InitGame();
	void InitMenu();
	void InitHelp();
	void InitStats();
};