#include "move.h"
#include "game.h"
#include <string>

void Move::set(std::string notation) {
	int start_column = notation[0] - 'a';
	int start_row = notation[1] - '1';

	int end_column = notation[2] - 'a';
	int end_row = notation[3] - '1';

	start = start_column + start_row * 8;
	end = end_column + end_row * 8;
}

bool Move::is_legal() {
	Board successors[50];
	int num_successors = game.board.set_successors(successors);

	Board new_board = game.board;
	new_board.make_move(start, end);

	for (int i = 0; i < num_successors; i++) {
		if (successors[i] == new_board)
			return true;
	}

	return false;
}

std::string Move::to_string() const {
	char start_column, end_row, end_column;
	end_row = end / 8 + '1';

	start_column = start % 8 + 'a';
	end_column = end % 8 + 'a';

	std::string result = "";

	if(start_column != end_column)
		result += start_column;

	if (capture)
		result += 'x';


	result += end_column;
	result += end_row;

	return result;
}

std::ostream& operator<<(std::ostream &os, const Move &move) {
	return os << move.to_string();
}