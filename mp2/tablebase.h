#pragma once
#include "move.h"
#include <unordered_map>
#include <my_global.h>
#include <mysql.h>

class TableBase {
private:
	std::unordered_map<int64_t, int64_t> database;
	int columns;
	int rows;

	static void handle_database_error(MYSQL *con);
	static bool create_database_table(const char* tablename);
public:
	TableBase(int rows, int columns);
	void put(int64_t key, int64_t value);
	void put(int64_t key, Move move, int64_t eval);
	Move get_move(int64_t key);
	int get_eval(int64_t key);
	uint64_t size();

	bool write_to_database(const char* tablename);
	bool read_from_database(const char* tablename);
};