#include "game.h"

bool SortTiles(actor* a, actor* b)
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
bool sortZlayer(actor* a, actor* b)
{
	return (a->zLayer < b->zLayer);
}
//Required
game::game()
{
	windowRef = nullptr;
	debugMode = false;
}
void game::Init(sf::RenderWindow& window)
{
	metrics.Load("Data/Stats.db");
	switch (state)
	{
	case GameState::menu:
		InitMenu();
		break;
	case GameState::play:
		InitGame();
		break;
	case GameState::help:
		InitHelp();
		break;
	case GameState::stats:
		InitStats();
		break;
	default:
		break;
	}
}
void game::Update(sf::RenderWindow& window, float elapsed)
{
	CheckMouseCollisions();

	//Loop through actor list, check collisions and update
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->Update(elapsed);
	}
	for (int i = 0; i < uiActors.size(); i++)
	{
		uiActors[i]->Update(elapsed);
	}
	if (clickedAnywhere)
	{
		time += elapsed;
	}
}
void game::Render(sf::RenderWindow& window, float elapsed)
{
	//Loop through list of every actor and draw them in order of their z layer (higher z = on top)
	std::sort(allActors.begin(), allActors.end(), sortZlayer);
	for (int i = 0; i < allActors.size(); i++)
	{
		allActors[i]->Render(window);
	}
	//If debugmode is active, loop through all collidable actors and draw hitboxes
	if (debugMode)
	{
		for (int i = 0; i < actors.size(); i++)
		{
			actors[i]->DisplayHitbox(*windowRef);
		}
	}
	
}
void game::CheckMouseCollisions()
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (IsMouseColliding(*actors[i], *windowRef))
		{
			if (!actors[i]->hovered)
			{
				actors[i]->hovered = true;
			}
		}
		else if (actors[i]->hovered)
		{
			actors[i]->hovered = false;
		}
	}
}

//Actor Management
void game::AddActor(actor* actor, bool collidable)
{
	actor->gameInst = this;
	actor->Init();
	if (collidable)
	{
		actors.push_back(actor);
	}
	else
	{
		uiActors.push_back(actor);
	}
	allActors.push_back(actor);
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
	//Loops through all uiActors
	for (int i = 0; i < uiActors.size(); i++)
	{
		delete uiActors[i];
		uiActors[i] = nullptr;
	}
	uiActors.clear();
	//Clear list of all actors
	allActors.clear();
}

//Minesweeper Related
void game::GenerateField() //Creates a grid of tiles size depending on fieldSize variable
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
			AddActor(tileObj);
		}
	}
}
void game::AddMines(int gridLoc) //Add mines randomly throughout the grid but not in a 3*3 area around first click
{
	//Get all the tile actors in a vector
	std::vector<tile*> tiles = GetAllTiles(true);
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(gridLoc, tiles);

	//Remove clicked tile from list
	tiles[gridLoc] = nullptr;
	//Loop through surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		//Remove surrounding tiles from list
		tiles[surrounding[i]->gridLoc] = nullptr;
	}
	//Set the amount of tiles removed to size of surrounding + 1
	int removable = surrounding.size() + 1;
	//Loop through list of tiles
	for (int i = 0; i < tiles.size(); i++)
	{
		//If tile has been removed take it out the list
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
	//Randomly assign bomb ids to some of the remaining tiles
	for (int i = 0; i < mineCount; i++)
	{
		int selected = rand() % (tiles.size() - 1);
		tiles[selected]->id = 11;
		tiles.erase(tiles.begin() + selected);
	}
}
void game::SetNumbers() //Calculate how many bombs are around each tile and set IDs accordingly
{
	std::vector<tile*> tiles = GetAllTiles();

	//Sort tiles in order going from top left to top right
	std::sort(tiles.begin(), tiles.end(), SortTiles);

	//Loop through them to set what number they will be
	for (int i = 0; i < tiles.size(); i++)
	{
		if (tiles[i]->id != 11)
		{
			int surroundingBombs = 0;
			//Get all surrounding tiles around current tile
			std::vector<tile*> surroundingTiles = GetSurroundingTiles(i, tiles);
			//Loop through surrounding tiles
			for (int x = 0; x < surroundingTiles.size(); x++)
			{
				//If surrounding tile is a bomb, add 1 to the surrounding bombs
				if (surroundingTiles[x]->id == 11)
				{
					surroundingBombs++;
				}
			}
			//Changes id depending on how many bombs surround the tile
			if (surroundingBombs > 0)
			{
				tiles[i]->id = surroundingBombs; //ID for numbered tile depending on amount of bombs surrounding
			}
			else
			{
				tiles[i]->id = 9; //ID for empty tile
			}
		}
	}
}
void game::OpenSurroundingEmpties(int tileLoc) //Open all surrounding tiles if they're empty
{
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(tileLoc);
	//Loop through all surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		if (surrounding[i]->id == 9)
		{
			//Open tile if it is empty
			surrounding[i]->OnLeftClick();
		}
	}
}
void game::OpenSurroundingTiles(int tileLoc) //Open all surrounding tiles no matter what
{
	//Get all surrounding tiles
	std::vector<tile*> surrounding = GetSurroundingTiles(tileLoc);
	//Loop through all surrounding tiles
	for (int i = 0; i < surrounding.size(); i++)
	{
		//Open surrounding tiles
		surrounding[i]->OnLeftClick();
	}
}
void game::PlayGame(sf::Vector2i size, int count)
{
	fieldSize = size;
	mineCount = count;

	state = play;
	Init(*windowRef);
}
std::vector<tile*> game::GetAllTiles(bool sort)
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

	if (sort)
	{
		std::sort(tiles.begin(), tiles.end(), SortTiles);
	}
	return tiles;
}
std::vector<tile*> game::GetSurroundingTiles(int tileLoc) //Gets all surrounding tiles
{
	std::vector<tile*> tiles = GetAllTiles(true);
	std::vector<tile*> surrounding;
	if (!TileOnTop(tileLoc)) //Check above
	{
		surrounding.push_back(tiles[tileLoc - fieldSize.x]);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x + 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x - 1)]);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		surrounding.push_back(tiles[tileLoc - 1]);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		surrounding.push_back(tiles[tileLoc + 1]);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		surrounding.push_back(tiles[tileLoc + fieldSize.x]);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x - 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x + 1)]);
		}
	}

	return surrounding;
}
std::vector<tile*> game::GetSurroundingTiles(int tileLoc, std::vector<tile*> tiles) //Gets all surrounding tiles but doesn't fetch each tile everytime
{
	std::vector<tile*> surrounding;
	if (!TileOnTop(tileLoc)) //Check above
	{
		surrounding.push_back(tiles[tileLoc - fieldSize.x]);

		if (!TileOnLeft(tileLoc)) //Check top left
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x + 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check top right
		{
			surrounding.push_back(tiles[tileLoc - (fieldSize.x - 1)]);
		}
	}
	if (!TileOnLeft(tileLoc)) //Check left
	{
		surrounding.push_back(tiles[tileLoc - 1]);
	}
	if (!TileOnRight(tileLoc)) //Check right
	{
		surrounding.push_back(tiles[tileLoc + 1]);
	}
	if (!TileOnBot(tileLoc)) //Check below
	{

		surrounding.push_back(tiles[tileLoc + fieldSize.x]);
		if (!TileOnLeft(tileLoc)) //Check down left
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x - 1)]);
		}
		if (!TileOnRight(tileLoc)) //Check down right
		{
			surrounding.push_back(tiles[tileLoc + (fieldSize.x + 1)]);
		}
	}

	return surrounding;
}
//Tile Location Checking
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
//Game Ending
void game::GameOver()
{
	canClick = false;
	clickedAnywhere = false;

	std::vector<tile*> tiles = GetAllTiles();
	for (int i = 0; i < tiles.size(); i++)
	{
		//Correctly flagged bomb
		if ((tiles[i]->id == 11) && (tiles[i]->flagged))
		{
			tiles[i]->id = 14;
			tiles[i]->revealed = true;
		}
		//Incorrectly flagged bomb
		else if ((tiles[i]->id != 11) && (tiles[i]->flagged))
		{
			tiles[i]->id = 13;
			tiles[i]->revealed = true;
		}
		//Revealed bomb
		else if ((tiles[i]->id == 11) && (!tiles[i]->revealed))
		{
			tiles[i]->revealed = true;
		}
	}
	metrics.StoreCurrentData();
	metrics.DBSave("Data/Stats.db");
	AddFinishButtons();
}
void game::GameWin()
{
	canClick = false;
	clickedAnywhere = false;
	//Adds 1 to completions for current difficulty
	metrics.currentData.completions++;
	//Overwrite time for current difficulty if time has been improved
	if ((time < metrics.currentData.time) || (metrics.currentData.time == NULL))
	{
		metrics.currentData.time = time;
	}
	//Store current data back into the stored data
	metrics.StoreCurrentData();
	metrics.DBSave("Data/Stats.db");
	AddFinishButtons();
}
void game::AddFinishButtons()
{
	retryButton* retry = nullptr;
	retry = new retryButton;
	retry->location = { windowRef->getSize().x / 2.f, 50};
	AddActor(retry);

	menuButton* menu = nullptr;
	menu = new menuButton;
	menu->location = { (windowRef->getSize().x / 2.f) + 200, 50 };
	AddActor(menu);

	exitButton* exit = nullptr;
	exit = new exitButton;
	exit->location = { (windowRef->getSize().x / 2.f) + 400, 50 };
	AddActor(exit);
}
//Scene Initialising
void game::InitGame()
{
	//Clear all actors and remove them from heap
	ClearActors();

	canClick = true;
	clickedAnywhere = false;
	tilesToWin = (fieldSize.x * fieldSize.y) - mineCount;
	time = 0.f;
	remainingFlags = mineCount;

	//Create background object
	image* BG = nullptr;
	BG = new image("Backgrounds/SweeperBG.png");
	BG->scaleType = ScaleMode::stretch;
	BG->origin = { 0, 0 };
	BG->zLayer = 0;
	AddActor(BG, false);

	//Adds tiles to scene
	GenerateField();

	timer* gameTimer = nullptr;
	gameTimer = new timer;
	gameTimer->location = { 600, 50 };
	gameTimer->scale = { 3, 3 };
	AddActor(gameTimer, false);

	flagCount* flagCounter = nullptr;
	flagCounter = new flagCount;
	flagCounter->location = { 400, 50 };
	flagCounter->scale = { 3, 3 };
	AddActor(flagCounter, false);

	//Gets data from selected difficulty
	metrics.GetCurrentData();
}
void game::InitMenu()
{
	ClearActors();

	image* bg = nullptr;
	bg = new image("Backgrounds/MinesweeperPlainBG.png");
	bg->scaleType = stretch;
	bg->origin = { 0, 0 };
	bg->location = { 0, 0 };
	bg->zLayer = 0;
	AddActor(bg, false);

	playButton* easyButton = nullptr;
	easyButton = new playButton;
	easyButton->location = { 220, 400 };
	easyButton->size = { 9, 9 };
	easyButton->count = 10;
	easyButton->text = "Easy";
	easyButton->difficulty = "Easy";
	AddActor(easyButton);
	
	playButton* normalButton = nullptr;
	normalButton = new playButton;
	normalButton->location = { 220, 600 };
	normalButton->size = { 16, 16 };
	normalButton->count = 40;
	normalButton->text = "Normal";
	normalButton->difficulty = "Normal";
	AddActor(normalButton);

	playButton* hardButton = nullptr;
	hardButton = new playButton;
	hardButton->location = { 220, 800 };
	hardButton->size = { 30, 16 };
	hardButton->count = 99;
	hardButton->text = "Hard";
	hardButton->difficulty = "Hard";
	AddActor(hardButton);

	//Stats button
	statsButton* stats = nullptr;
	stats = new statsButton;
	stats->location = { 1380, 500 };
	stats->scale = { 3, 3 };
	AddActor(stats);

	//Help button
	helpButton* help = nullptr;
	help = new helpButton;
	help->location = { 1380, 700 };
	help->scale = { 3, 3 };
	AddActor(help);
}
void game::InitHelp()
{
	ClearActors();

	image* bg = nullptr;
	bg = new image("Backgrounds/MinesweeperHelpBG.png");
	bg->scaleType = stretch;
	bg->origin = { 0, 0 };
	bg->location = { 0, 0 };
	AddActor(bg, false);

	//Main menu button
	menuButton* menu = nullptr;
	menu = new menuButton;
	menu->location = { (windowRef->getSize().x / 2.f), 800 };
	AddActor(menu);
}
void game::InitStats()
{
	ClearActors();

	//Create background image and place it
	image* bg = nullptr;
	bg = new image("Backgrounds/MinesweeperStatsBG.png");
	bg->scaleType = stretch;
	bg->origin = { 0, 0 };
	bg->location = { 0, 0 };
	AddActor(bg, false);

	//Create text for easy stats
	textBox* easyStats = nullptr;
	easyStats = new textBox;
	easyStats->textColour = sf::Color::Black;
	easyStats->location = { 300, 400 };
	//Fetch easy stats
	Metrics::difficultyData easyData = metrics.GetDifficultyData("Easy");
	//Round time to 2 decimal place
	std::stringstream easyStream;
	float easyTime = easyData.time;
	easyStream << std::fixed << std::setprecision(2) << easyTime;
	//Place easy stats into text
	easyStats->string = "\nBest Time: " + easyStream.str();
	easyStats->string += "\n\nCompletions: " + std::to_string(easyData.completions);
	easyStats->string += "\n\nAttempts: " + std::to_string(easyData.attempts);
	AddActor(easyStats, false);
	
	//Create text for normal stats
	textBox* normalStats = nullptr;
	normalStats = new textBox;
	normalStats->textColour = sf::Color::Black;
	normalStats->location = { 800, 400 };
	//Fetch normal stats
	Metrics::difficultyData normalData = metrics.GetDifficultyData("Normal");
	//Round time to 2 decimal place
	std::stringstream normalStream;
	float normalTime = normalData.time;
	normalStream << std::fixed << std::setprecision(2) << normalTime;
	//Place normal stats into text
	normalStats->string = "\nBest Time: " + normalStream.str();
	normalStats->string += "\n\nCompletions: " + std::to_string(normalData.completions);
	normalStats->string += "\n\nAttempts: " + std::to_string(normalData.attempts);
	AddActor(normalStats, false);
	
	//Create text for hard stats
	textBox* hardStats = nullptr;
	hardStats = new textBox;
	hardStats->textColour = sf::Color::Black;
	hardStats->location = { 1300, 400 };
	//Fetch hard stats
	Metrics::difficultyData hardData = metrics.GetDifficultyData("Hard");
	//Round time to 2 decimal place
	std::stringstream hardStream;
	float hardTime = hardData.time;
	hardStream << std::fixed << std::setprecision(2) << hardTime;
	//Place hard stats into text
	hardStats->string = "\nBest Time: " + hardStream.str();
	hardStats->string += "\n\nCompletions: " + std::to_string(hardData.completions);
	hardStats->string += "\n\nAttempts: " + std::to_string(hardData.attempts);
	AddActor(hardStats, false);

	//Main menu button
	menuButton* menu = nullptr;
	menu = new menuButton;
	menu->location = { (windowRef->getSize().x / 2.f), 800 };
	AddActor(menu);
}