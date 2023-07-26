#include "database.h"
#include <assert.h>
#include <fstream>

int LoadOrSaveDb(sqlite3* InMemory, const std::string& FileName, bool saveToHDD)
{
	int rc; //Return code of method
	sqlite3* FileRef; //File opened from FileName
	sqlite3_backup* BackupRef; //Backup used to copy data
	sqlite3* ToRef;
	sqlite3* FromRef;


	//Open the database file, exit if any fails happen
	rc = sqlite3_open(FileName.c_str(), &FileRef);
	if (rc == SQLITE_OK)
	{
		FromRef = (saveToHDD ? InMemory : FileRef);
		ToRef = (saveToHDD ? FileRef : InMemory);

		BackupRef = sqlite3_backup_init(ToRef, "main", FromRef, "main"); //Create backup object
		if (BackupRef)
		{
			sqlite3_backup_step(BackupRef, -1); //Copies data from fileRef to InMemory
			sqlite3_backup_finish(BackupRef); //Releases resources associated with backup object
		}
		rc = sqlite3_errcode(ToRef); //If no errors this sets to SQLITE_OK
	}

	sqlite3_close(FileRef); //Close connection to database
	return rc; //Return result code
}

static int Database_Callback(void* Stuff, int argc, char** argv, char** ColumnName)
{
	assert(Stuff);
	database* p = reinterpret_cast<database*>(Stuff);
	return p->Callback(argc, argv, ColumnName);
}

void database::Init(const std::string& _dbFileName, bool& doesExist)
{
	dbFileName = _dbFileName;

	if (sqlite3_open(":memory:", &dbRef))
	{
		assert(false);
	}
	doesExist = false;
	std::ifstream f(dbFileName.c_str());
	if (f.good())
	{
		f.close();
		int rc = LoadOrSaveDb(dbRef, dbFileName.c_str(), false);
		if (rc != SQLITE_OK)
		{
			assert(false);
		}
		doesExist = true;
	}
}

void database::SaveToDisk()
{
	assert(dbRef && !dbFileName.empty());
	int rc = LoadOrSaveDb(dbRef, dbFileName.c_str(), true);
	if (rc != SQLITE_OK) {
		assert(false);
	}
}

void database::Close()
{
	sqlite3_close(dbRef);
	dbRef = nullptr;
}

bool database::ExecQuery(const std::string& query)
{
	results.clear();
	char* zErrMsg = 0;
	int rc = sqlite3_exec(dbRef, query.c_str(), Database_Callback, this, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		assert(false);
		return false;
	}
	return true;
}


const std::string& database::GetStr(int rowNum, const std::string& fieldName)
{
	row& r = results.at(rowNum);
	size_t id = 0;
	while (id < r.size() && r[id].name != fieldName) {
		++id;
	}
	assert(id < r.size());
	return r[id].value;
}
float database::GetFloat(int rowNum, const std::string& fieldName)
{
	std::string res = GetStr(rowNum, fieldName);
	return stof(res);
}
int database::GetInt(int rowNum, const std::string& fieldName)
{
	std::string res = GetStr(rowNum, fieldName);
	return stoi(res);
}


std::vector<std::string> database::GetFieldNames(const std::string& table)
{
	std::string sql = "SELECT * FROM " + table;
	std::vector<std::string> fields;
	sqlite3_stmt* res;
	int rc = sqlite3_prepare_v2(dbRef, sql.c_str(), -1, &res, 0);
	if (rc == SQLITE_OK)
	{
		for (int i = 0; i < sqlite3_column_count(res); ++i)
			fields.push_back(sqlite3_column_name(res, i));
	}
	sqlite3_finalize(res);
	return fields;
}

int database::Callback(int argc, char** argv, char** azColName)
{
	row Row;
	for (int i = 0; i < argc; i++) {
		Row.push_back(Field{ azColName[i], (argv[i] ? argv[i] : "NULL") });
	}
	results.push_back(Row);
	return 0;
}