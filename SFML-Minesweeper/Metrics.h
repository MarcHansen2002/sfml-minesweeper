#pragma once
#include <iostream>
#include <sstream>
#include <iomanip>
#include <assert.h>
//My Files
#include "database.h"

class Metrics
{
public:
	const std::string VERSION = "1.0";

	bool useDB = true;
	database db;

	struct difficultyData
	{
		std::string difficultyName = "na";
		float time;
		int attempts;
		int completions;
	};
	difficultyData currentData;
	std::vector<difficultyData> diffData;
	std::string filePath;

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