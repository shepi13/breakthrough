#pragma once
#include "board.h"

namespace Eval {
	int pop_count(uint64_t bitboard);
	float defensive_eval_1(const Board *board, int side);
	float defensive_eval_2(const Board *board, int side);
	float offensive_eval_1(const Board *board, int side);
	float offensive_eval_2(const Board *board, int side);
	float basic_eval(const Board *board, int side);
}