#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "../entities/Player.h"
#include <map>
#include <string>
#include <assert.h>
#include <sqlite3.h>
#include <iostream>
#include <filesystem>

class DataHandler {

public:
	DataHandler();

private:

	sqlite3* playerDB;
	sqlite3* keyDB;

	static DataHandler* sInstance;

	void executeSQLStatement(const char* statement, sqlite3* database);
};

#endif // !DATAHANDLER_H

