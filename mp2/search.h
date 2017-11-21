#pragma once
#include "board.h"

extern bool running;

namespace Search {
	void insertion_sort(Board* successors, int num_successors);

	float minimax(Board board, int depth);
	float optimized_minimax(Board board, int depth);
	float alpha_beta(Board b, int depth, float alpha, float beta);
	float optimized_alpha_beta(Board b, int depth, float alpha, float beta);
	void iterative_deepening_alpha_beta(Board b);
	float timed_search(int duration);
}