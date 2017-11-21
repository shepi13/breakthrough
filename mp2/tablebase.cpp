#pragma comment(lib, "libmysql.lib")

#include "tablebase.h"
#include <iostream>
#include <cstdlib>
using namespace std;

TableBase::TableBase(int rows, int columns) {
	this->columns = columns;
	this->rows = rows;
}

void TableBase::put(int64_t key, int64_t value) {
	database[key] = value;
}

void TableBase::put(int64_t key, Move best_move, int64_t eval) {
	int end_row = best_move.end / columns;
	int end_col = best_move.end % columns;
	int start_row = best_move.start / columns;
	int start_col = best_move.start % columns;

	int64_t value = end_row + 8 * end_col + 64 * start_row + 512 * start_col + 4096 * eval;
	database[key] = value;
}

Move TableBase::get_move(int64_t key) {
	Move move;

	int64_t value = database.at(key);
	int end_row = value % columns;

	value /= columns;
	int end_col = value % columns;

	value /= columns;
	int start_row = value % columns;

	value /= columns;
	int start_col = value % columns;

	move.start = columns*start_row + start_col;
	move.end = columns*end_row + end_col;
	return move;
}

int TableBase::get_eval(int64_t key) {
		std::unordered_map<int64_t, int64_t>::const_iterator found = database.find(key);

		if (found == database.end())
			return -1;

		return found->second / columns / columns / columns / columns;
}

uint64_t TableBase::size() {
	return database.size();
}

void TableBase::handle_database_error(MYSQL *con) {
	cout << mysql_error(con) << endl;
	mysql_close(con);
}

bool TableBase::create_database_table(const char* tablename) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL) {
		cout << mysql_error(con) << endl;
		return false;
	}

	if (mysql_real_connect(con, "localhost", "root", "herder13", "cs440", 3306, NULL, 0) == NULL) {
		handle_database_error(con);
		return false;
	}

	char drop_query[500] = "DROP TABLE IF EXISTS ";
	strcat_s(drop_query, tablename);

	if (mysql_query(con, drop_query)) {
		handle_database_error(con);
		return false;
	}

	char create_query[1000] = "CREATE TABLE ";
	strcat_s(create_query, tablename);
	strcat_s(create_query, "(\
		position BIGINT UNSIGNED NOT NULL,\
		eval INT NOT NULL,\
		start_row INT,\
		end_row INT,\
		start_column INT,\
		end_column INT,\
		PRIMARY KEY(position))");

	if (mysql_query(con, create_query)) 
	{
		handle_database_error(con);
		return false;
	}

	mysql_close(con);
	return true;
}

bool TableBase::write_to_database(const char* tablename) {
	create_database_table(tablename);

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) {
		cout << mysql_error(con) << endl;
		return false;
	}

	if (mysql_real_connect(con, "localhost", "root", "herder13", "cs440", 3306, NULL, 0) == NULL) {
		handle_database_error(con);
		return false;
	}

	char initial_stmt1[500] = "INSERT INTO ";
	strcat_s(initial_stmt1, tablename);
	strcat_s(initial_stmt1, "(position, eval) VALUES ");
	char initial_stmt2[500] = "INSERT INTO ";
	strcat_s(initial_stmt2, tablename);
	strcat_s(initial_stmt2, "(position, eval, start_row, start_column, end_row, end_column) VALUES ");

	char stmt1[100000];
	char stmt2[100000]; 
	char values1[100][1000], values2[100][1000];
	int num1 = 0, num2 = 0;

	strcpy_s(stmt1, initial_stmt1);
	strcpy_s(stmt2, initial_stmt2);

	for (auto const& entry : database) {
		char position_string[1000];
		uint64_t position;
		int eval, start_row, end_row, start_column, end_column;
		Move move;

		position = entry.first;
		eval = get_eval(position);
		_ui64toa_s(position, position_string, 100, 10);

		if (eval == 1000) {
			sprintf_s(values1[num1], 1000, "(%s, %d),\n", position_string, eval);
			num1++;
		}
		else {
			move = get_move(position);
			start_row = move.start / columns;
			start_column = move.start % columns;
			end_row = move.end / columns;
			end_column = move.end % columns;

			sprintf_s(values2[num2], 1000, "(%s, %d, %d, %d, %d, %d),\n",
				position_string, eval, start_row, start_column, end_row, end_column);
			num2++;
		}

		if (num1 == 100) {
			for (int i = 0; i < 100; i++)
				strcat_s(stmt1, 100000, values1[i]);

			stmt1[strlen(stmt1) - 2] = 0;
			if (mysql_query(con, stmt1)) {
				handle_database_error(con);
				return false;
			}

			strcpy_s(stmt1, initial_stmt1);
			num1 = 0;
		}

		if (num2 == 100) {
			for (int i = 0; i < 100; i++)
				strcat_s(stmt2, 100000, values2[i]);

			stmt2[strlen(stmt2) - 2] = 0;
			if (mysql_query(con, stmt2)) {
				handle_database_error(con);
				return false;
			}

			strcpy_s(stmt2, initial_stmt2);
			num2 = 0;
		}
	}

	for (int i = 0; i < num1; i++)
		strcat_s(stmt1, 100000, values1[i]);

	stmt1[strlen(stmt1) - 2] = 0;

	if (mysql_query(con, stmt1)) {
		handle_database_error(con);
		return false;
	}

	for (int i = 0; i < num2; i++)
		strcat_s(stmt2, 100000, values2[i]);

	stmt2[strlen(stmt2) - 2] = 0;

	if (mysql_query(con, stmt2)) {
		handle_database_error(con);
		return false;
	}

	mysql_close(con);
	return true;
}

bool TableBase::read_from_database(const char* tablename) {
	MYSQL *con = mysql_init(NULL);

	if (con == NULL) {
		cout << mysql_error(con) << endl;
		return false;
	}

	if (mysql_real_connect(con, "localhost", "root", "herder13", "cs440", 3306, NULL, 0) == NULL) {
		handle_database_error(con);
	}

	char select_query[100] = "SELECT * FROM ";
	strcat_s(select_query, tablename);

	if (mysql_query(con, select_query)) {
		handle_database_error(con);
		return false;
	}

	MYSQL_RES *result = mysql_store_result(con);

	if (result == NULL) {
		handle_database_error(con);
		return false;
	}

	database.reserve(database.size() + mysql_num_rows(result));

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(result))) {
		Move move;
		int64_t position = _atoi64(row[0]);
		int64_t eval = _atoi64(row[1]);
		int start_row = atoi(row[2]);
		int start_column = atoi(row[3]);
		int end_row = atoi(row[4]);
		int end_column = atoi(row[5]);
		move.start = start_row * 8 + start_column;
		move.end = end_row * 8 + end_column;
		put(position, move, eval);
	}

	return true;
}