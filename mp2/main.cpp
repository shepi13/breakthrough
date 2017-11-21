#include <iostream>
#include "board.h"
#include "move.h"
#include "game.h"
#include "search.h"
#include <windows.h>
#include <vector>
#include <time.h>
#include <string>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;

int main(int argv, char** argc) {
	float eval = 0;
	Move move;
	std::string move_string;
	int white_wins = 0, black_wins = 0;
	double white_time = 0.0, black_time = 0.0;  

	game.reset();

	game.board.print_board();
	cout << endl;
	while (game.result() == 0) {
		if (game.board.side == 0) {
			cout  << "Enter move: ";
			cin >> move_string;
			std::transform(move_string.begin(), move_string.end(), move_string.begin(), ::tolower);
			move.set(move_string);

			if (move_string == "u" || move_string == "undo") {
				game.undo_move();
				game.undo_move();
			}
			else if (!move.is_legal()) {
				cout << "Illegal move entered, ";
				continue;
			}
			else {
				game.make_move(move);
			}

			system("cls");
			game.board.print_board();
		} 
		else {
			game.transposition_table.reset();
			eval = Search::timed_search(20000);
			game.make_move(game.get_best_move());
			system("cls");
			game.board.print_board();
			cout <<  "Eval: " << eval << endl << "Depth: " << game.depth << endl << endl;
		}
	}

	system("pause");
	return 0;
}