#include "eval.h"
#include "game.h"
#include <cstdlib>

int Eval::pop_count(uint64_t bitboard) {
	bitboard = bitboard - ((bitboard >> 1) & 0x5555555555555555);
	bitboard = (bitboard & 0x3333333333333333) + ((bitboard >> 2) & 0x3333333333333333);
	return (((bitboard + (bitboard >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
}

float Eval::defensive_eval_1(const Board *board, int side) {
	double r = game.dist(game.rng);

	if (side == 0) {
		return pop_count(board->white_pieces) * 2 + r;
	}
	else {
		return pop_count(board->black_pieces) * 2 + r;
	}
}

float Eval::offensive_eval_1(const Board *board, int side) {
	double r = game.dist(game.rng);

	if (side == 1) {
		return (30 - pop_count(board->white_pieces)) * 2 + r;
	}
	else {
		return (30 - pop_count(board->black_pieces)) * 2 + r;
	}
}

float Eval::defensive_eval_2(const Board *board, int side) {
	double r = game.dist(game.rng);
	float eval;

	if (side == 0) {
		eval = 30 - pop_count(board->black_pieces);

		uint64_t pieces = board->white_pieces;
		for (int i = 0; i < 8; i++) {
			eval += pop_count(pieces & 0xffLL) * (2 - (static_cast<float>(i) / static_cast<float>(10)));
			pieces = pieces >> 8;
		}
	}
	else {
		eval = 30 - pop_count(board->white_pieces);

		uint64_t pieces = board->black_pieces;
		for(int i = 0; i < 8; i++) {
			eval += pop_count(pieces & 0xffLL) * (1.3 + (static_cast<float>(i) / static_cast<float>(10)));
			pieces = pieces >> 8;
		}
	}

	eval *= 10;
	eval += r;
	return eval;
}

float Eval::offensive_eval_2(const Board *board, int side) {
	double r = game.dist(game.rng);
	float eval;

	if (side == 0) {
		eval = 30 - pop_count(board->black_pieces);
		uint64_t pieces = board->white_pieces;
		for (int i = 0; i < 8; i++) {
			eval += pop_count(pieces & 0xffLL) * (0.4 + (static_cast<float>(i) / static_cast<float>(10)));
			pieces = pieces >> 8;
		}
	}
	else {
		eval = 30 - pop_count(board->white_pieces);
		uint64_t pieces = board->black_pieces;
		for (int i = 0; i < 8; i++) {
			eval += pop_count(pieces & 0xffLL) * (1.1 - (static_cast<float>(i) / static_cast<float>(10)));
			pieces = pieces >> 8;
		}
	}

	eval *= 10;
	eval += r;
	return eval;
}

float Eval::basic_eval(const Board *board, int side) {
	double r = game.dist(game.rng);
	float eval;

	int white_count = pop_count(board->white_pieces);
	int black_count = pop_count(board->black_pieces);

	if (side == 0)
		eval = white_count - black_count;
	else
		eval = black_count - white_count;

	return eval + r;
}