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

	sqlite3* db;

	static DataHandler* sInstance;

	void executeSQLStatement(const char* statement);
};

#endif // !DATAHANDLER_H

