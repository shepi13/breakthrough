#include "search.h"
#include "game.h"
#include <thread>
#include <iostream>

bool running = true;

float Search::minimax(Board board, int depth) {
	if (board.side == 0)
		game.white_nodes_expanded++;
	else
		game.black_nodes_expanded++;

	float result = board.get_result_no_tables();

	if (result != -1 && board.zobrist != game.board.zobrist) {
		return result;
	}

	if (depth == 0)
		return board.eval();

	Board successors[50];	
	int best_idx = 0;
	int num_successors = board.set_successors(successors);

	//Min node
	if (board.moves % 2 != board.side) {
		float min_eval = 2000;

		for (int i = 0; i < num_successors; i++) {
			float next_eval = minimax(successors[i], depth - 1);
			if (next_eval < min_eval) {
				min_eval = next_eval;
				best_idx = i;
			}
		}

		game.best_successor = successors[best_idx];
		return min_eval;
	}
	//Max node
	else {
		float max_eval = -2000;

		for (int i = 0; i < num_successors; i++) {
			float next_eval = minimax(successors[i], depth - 1);
			if (next_eval > max_eval) {
				max_eval = next_eval;
				best_idx = i;
			}
		}

		game.best_successor = successors[best_idx];
		return max_eval;
	}
}

float Search::optimized_minimax(Board board, int depth) {
	if (board.side == 0)
		game.white_nodes_expanded++;
	else
		game.black_nodes_expanded++;

	float result = board.get_result(depth);

	if (result != -1 && board.zobrist != game.board.zobrist)
		return result;

	if (depth == 0)
		return board.eval();

	Board successors[50];
	int best_idx = 0;
	int num_successors = board.set_successors(successors);

	//Min node
	if (board.moves % 2 != board.side) {
		float min_eval = 2000;

		for (int i = 0; i < num_successors; i++) {
			float next_eval = optimized_minimax(successors[i], depth - 1);
			if (next_eval < min_eval) {
				min_eval = next_eval;
				best_idx = i;
			}
		}

		game.best_successor = successors[best_idx];
		game.transposition_table.put(HashEntry(board.zobrist, min_eval, depth, EXACT));
		return min_eval;
	}
	//Max node
	else {
		float max_eval = -2000;

		for (int i = 0; i < num_successors; i++) {
			float next_eval = optimized_minimax(successors[i], depth - 1);
			if (next_eval > max_eval) {
				max_eval = next_eval;
				best_idx = i;
			}
		}

		game.best_successor = successors[best_idx];
		game.transposition_table.put(HashEntry(board.zobrist, max_eval, depth, EXACT));
		return max_eval;
	}
}

float Search::alpha_beta(Board board, int depth, float alpha, float beta) {
	if (board.side == 0)
		game.white_nodes_expanded++;
	else
		game.black_nodes_expanded++;

	float result = board.get_result(depth);

	if (result != -1 && board.zobrist != game.board.zobrist)
		return result;

	if (depth == 0)
		return board.eval();

	Board successors[50];
	int best_idx = 0;
	int num_successors = board.set_successors(successors);

	//Min node
	if (board.moves % 2 != board.side) {
		for (int i = 0; i < num_successors; i++) {
			float next_eval = alpha_beta(successors[i], depth - 1, alpha, beta);

			if (next_eval < beta) {
				beta = next_eval;
				best_idx = i;
			}
			
			if (alpha >= beta) {
				game.best_successor = successors[best_idx];
				return beta;
			}
		}

		game.best_successor = successors[best_idx];
		return beta;
	}
	//Max node
	else {
		for (int i = 0; i < num_successors; i++) {
			float next_eval = alpha_beta(successors[i], depth - 1, alpha, beta);

			if (next_eval > alpha) {
				alpha = next_eval;
				best_idx = i;
			}

			if (alpha >= beta) {
				game.best_successor = successors[best_idx];
				return alpha;
			}
		}

		game.best_successor = successors[best_idx];
		return alpha;
	}
}

float Search::optimized_alpha_beta(Board board, int depth, float alpha, float beta) {
	if (running == false)
		return -10001;

	if (board.side == 0)
		game.white_nodes_expanded++;
	else
		game.black_nodes_expanded++;

	float result = board.get_result(depth);

	if (result != -1 && board.zobrist != game.board.zobrist)
		return result;

	if (depth == 0)
		return board.eval();

	HashEntry entry = game.transposition_table.get(board.zobrist);

	if (entry.type == EXACT && entry.depth >= depth && board.zobrist != game.board.zobrist)
		return entry.eval;
	else if (entry.type == FAILLOW && entry.depth >= depth && board.zobrist != game.board.zobrist)
		alpha = entry.eval;
	else if (entry.type == FAILHIGH && entry.depth >= depth && board.zobrist != game.board.zobrist)
		beta = entry.eval;

	if (alpha >= beta)
		return entry.eval;

	Board successors[50];
	int best_idx = 0;
	int current_idx = 0;
	int num_successors = board.set_successors(successors);
	//insertion_sort(successors, num_successors);

	//Min node
	if (board.moves % 2 != board.side) {
		for (int i = 0; i < num_successors; i++) {
			float next_eval = optimized_alpha_beta(successors[i], depth - 1, alpha, beta);

			if (next_eval < beta) {
				beta = next_eval;
				best_idx = i;
			}

			if (alpha >= beta) {
				game.best_successor = successors[best_idx];
				game.transposition_table.put(HashEntry(board.zobrist, beta, depth, FAILLOW));
				return beta;
			}
		}

		game.best_successor = successors[best_idx];
		game.transposition_table.put(HashEntry(board.zobrist, beta, depth, EXACT));
		return beta;
	}
	//Max node
	else {
		for (int i = num_successors - 1; i >= 0; i--) {
			float next_eval = optimized_alpha_beta(successors[i], depth - 1, alpha, beta);

			if (next_eval > alpha) {
				alpha = next_eval;
				best_idx = i;
			}

			if (alpha >= beta) {
				game.best_successor = successors[best_idx];
				game.transposition_table.put(HashEntry(board.zobrist, alpha, depth, FAILHIGH));
				return alpha;
			}
		}

		game.best_successor = successors[best_idx];
		game.transposition_table.put(HashEntry(board.zobrist, alpha, depth, EXACT));
		return alpha;
	}
}

void Search::insertion_sort(Board *successors, int num_successors) {
	for (int i = 0; i < num_successors; i++) {
		for (int j = i; j > 0 && successors[j] < successors[j-1]; j--) {
			Board temp = successors[j];
			successors[j] = successors[j - 1];
			successors[j - 1] = temp;
		}
	}
}

void Search::iterative_deepening_alpha_beta(Board b) {
	game.eval = optimized_alpha_beta(b, 1, -10000, 10000);
	Move move = game.get_best_move();

	int depth = 2;
	while (depth < 1000) {
		float eval = optimized_alpha_beta(b, depth, -10000, 10000);

		if (running) {
			game.eval = eval;
			move = game.get_best_move();
		}
		else {
			game.best_successor = game.board;
			game.best_successor.make_move(move);
			game.depth = depth-1;
			return;
		}

		depth++;
	}
}

float Search::timed_search(int duration) {
	running = true;
	std::thread search(iterative_deepening_alpha_beta, game.board);
	std::this_thread::sleep_for(std::chrono::milliseconds(duration));
	running = false;

	search.join();

	return game.eval;
}