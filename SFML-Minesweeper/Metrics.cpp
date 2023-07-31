#include "Metrics.h"

bool Metrics::DBLoad(const std::string& path)
{
	diffData.clear();
	bool exists;
	db.Init(path, exists);
	//Load data if database exists
	if (exists)
	{
		//Fetch all data from GAME_INFO
		db.ExecQuery("SELECT * FROM GAME_INFO");
		std::string version = db.GetStr(0, "VERSION");
		if (version != Metrics::VERSION)
		{
			db.ExecQuery("DROP TABLE IF EIXSTS GAME_STATS");
			db.ExecQuery("DROP TABLE IF EXISTS GAME_INFO");
			exists = false;
		}
	}
	//Create database if one does not exist
	else
	{
		//Create stats table
		db.ExecQuery("CREATE TABLE GAME_STATS(" \
			"DIFFICULTY			TEXT	NOT NULL,"\
			"TIME		REAL		NOT NULL,"\
			"ATTEMPTS		INT		NOT NULL,"\
			"COMPLETIONS		INT		NOT NULL)");
		//Create info table
		db.ExecQuery("CREATE TABLE GAME_INFO("\
			"ID			INTEGER PRIMARY KEY		,"\
			"VERSION		CHAR(10)	NOT NULL)");

		std::stringstream ss;
		ss << "INSERT INTO GAME_INFO (VERSION)"\
			"VALUES (" << Metrics::VERSION << ")";
		db.ExecQuery(ss.str());
		return false;
	}
	//Get stats from DB, old or empty from newly created
	db.ExecQuery("SELECT * FROM GAME_STATS");
	//Loop through results and add them to stored vector
	for (size_t i = 0; i < db.results.size(); i++)
	{
		diffData.push_back(difficultyData{ db.GetStr(i,"DIFFICULTY"), db.GetFloat(i, "TIME"), db.GetInt(i, "ATTEMPTS"), db.GetInt(i, "COMPLETIONS") });
	}
	return false;
}
bool Metrics::DBSave(const std::string& path)
{
	db.ExecQuery("DELETE FROM GAME_STATS");
	std::stringstream ss;
	for (size_t i = 0; i < diffData.size(); i++)
	{
		ss.str("");
		ss << "INSERT INTO GAME_STATS (DIFFICULTY, TIME, ATTEMPTS, COMPLETIONS)" \
			<< "VALUES ('" << diffData[i].difficultyName << "'," \
			<< diffData[i].time << "," \
			<< diffData[i].attempts << "," \
			<< diffData[i].completions << ")";
		db.ExecQuery(ss.str());
	}
	ss.str("");
	ss << "UPDATE GAME_INFO SET VERSION = " << Metrics::VERSION;
	db.ExecQuery(ss.str());
	db.SaveToDisk();

	return false;
}

void Metrics::GetCurrentData()
{
	for (int i = 0; i < diffData.size(); i++)
	{
		//Makes sure it's loading from the correct difficulty
		if (diffData[i].difficultyName == currentData.difficultyName)
		{
			//Get data from this row
			currentData.time = diffData[i].time;
			currentData.attempts = diffData[i].attempts;
			currentData.completions = diffData[i].completions;
			//Exit loop
			i = diffData.size() + 1;
		}
	}
}
void Metrics::StoreCurrentData()
{
	bool found = false;
	for (int i = 0; i < diffData.size(); i++)
	{
		//Makes sure it's saving to the correct difficulty
		if (currentData.difficultyName == diffData[i].difficultyName)
		{
			found = true;
			//Store data into this row
			diffData[i].time = currentData.time;
			diffData[i].attempts = currentData.attempts;
			diffData[i].completions = currentData.completions;
			//Exit loop
			i = diffData.size() + 1;
		}
	}
	//If difficulty is not recorded. Add it in
	if (!found)
	{
		diffData.push_back(currentData);
	}
}

Metrics::difficultyData Metrics::GetDifficultyData(std::string diff)
{
	//Get data from selected difficulty
	for (int i = 0; i < diffData.size(); i++)
	{
		if (diffData[i].difficultyName == diff)
		{
			return diffData[i];
		}
	}
	//Return an empty stats list if not found
	difficultyData empty = { diff, 0, 0, 0 };
	return empty;
}
