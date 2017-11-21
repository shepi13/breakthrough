#pragma once
#include <cstdint>
#include "hash_table.h"
#include "move.h"

class Board
{
private:
	uint64_t generate_white_moves(uint64_t bitboard);
	uint64_t generate_black_moves(uint64_t bitboard);

	uint64_t generate_white_attacks(uint64_t bitboard);
	uint64_t generate_black_attacks(uint64_t bitboard);

	uint64_t get_white_hash() const;
	uint64_t get_black_hash() const;

	uint64_t int_pow(int a, int b) const;
public:
	bool marked;
	int moves;
	int side;
	uint64_t white_pieces;
	uint64_t black_pieces;
	uint64_t zobrist;

	Board();

	void print_board();
	static int bit_index(uint64_t bitboard);

	float get_result(int depth) const;
	float get_result_no_tables() const;
	float eval() const;

	void make_move(int start, int end);
	void make_move(Move move);
	int set_successors(Board *successors);

	bool operator<(const Board& other) const;
	bool operator==(const Board& other) const;
};