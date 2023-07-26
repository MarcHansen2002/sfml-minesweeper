#pragma once
#include "../sqlite/sqlite3.h"
#include <string>
#include <vector>

class database
{
public:
	//Variable
	sqlite3* dbRef = nullptr; //Reference to main database
	struct Field
	{
		std::string name; //Name of field
		std::string value; //Converts all fetched values to strings since we're unsure on datatype
	};
	typedef std::vector<Field> row; //A singular row from the results
	std::vector<row> results; //All the rows returned from the last query
	std::string dbFileName; //Directory of where data is stored

	//Functions

	//Opens the database, creates one if none exists
	void Init(const std::string& _dbFileName, bool& doesExist);
	//Saves database to hard drive
	void SaveToDisk();
	//Called whenever finished with database
	void Close();
	//Send a query to the database
	bool ExecQuery(const std::string& query);

	//convert a particular row+field string to the target type
	const std::string& GetStr(int rowNum, const std::string& fieldName);
	float GetFloat(int rowNum, const std::string& fieldName);
	int GetInt(int rowNum, const std::string& fieldName);

	//get all the field names in a specific table
	std::vector<std::string> GetFieldNames(const std::string& table);
	//the callback is used to get results back from the database
	int Callback(int argc, char** argv, char** azColName);

};

