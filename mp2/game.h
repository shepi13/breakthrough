#pragma once
#include "board.h"
#include "tablebase.h"
#include "eval.h"
#include "move.h"
#include <random>
#include <string>
#include <vector>

#define TABLE_NAME "nine_piece"

class Game {
public:
	Board board;
	std::vector<Board> prev_positions;

	uint64_t zobrist_piece_nums[64][2];
	uint64_t zobrist_side_to_move;

	HashTable transposition_table;
	TableBase tablebase;
	Board best_successor;

	std::string score;

	const uint64_t not_hfile = 0x7f7f7f7f7f7f7f7f;
	const uint64_t not_afile = 0xfefefefefefefefe;

	const int index64[64] = {
		0,  1, 48,  2, 57, 49, 28,  3,
		61, 58, 50, 42, 38, 29, 17,  4,
		62, 55, 59, 36, 53, 51, 43, 22,
		45, 39, 33, 30, 24, 18, 12,  5,
		63, 47, 56, 27, 60, 41, 37, 16,
		54, 35, 52, 21, 44, 32, 23, 11,
		46, 26, 40, 15, 34, 20, 31, 10,
		25, 14, 19,  9, 13,  8,  7,  6
	};

	int table_hits;
	int white_nodes_expanded;
	int black_nodes_expanded;
	int quiescence_nodes;
	int table_misses;

	float eval;
	int depth;

	float(*white_function)(const Board *, int) = Eval::offensive_eval_2;
	float(*black_function)(const Board *, int) = Eval::offensive_eval_2;

	std::mt19937 rng;
	std::uniform_real_distribution<double> dist;

	Game();
	void reset();

	int result();

	void make_move(int start, int end);
	void make_move(Move move);
	void undo_move();

	Move get_best_move();
};

extern Game game;