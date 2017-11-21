#include "board.h"
#include "game.h"

using std::cout;
using std::endl;

Board::Board() {
	side = 0;
	moves = 0;
	marked = false;
}

uint64_t Board::generate_white_moves(uint64_t bitboard) {
	return ((bitboard << 9 & game.not_afile) | (bitboard << 7 & game.not_hfile) |
		(bitboard << 8 & ~black_pieces)) & ~white_pieces;
}

uint64_t Board::generate_black_moves(uint64_t bitboard) {
	return ((bitboard >> 7 & game.not_afile) | (bitboard >> 9 & game.not_hfile) |
		(bitboard >> 8 & ~white_pieces)) & ~black_pieces;
}

uint64_t Board::generate_white_attacks(uint64_t bitboard) {
	return ((bitboard << 9 & game.not_afile) | (bitboard << 7 && game.not_hfile)) & black_pieces;
}

uint64_t Board::generate_black_attacks(uint64_t bitboard) {
	return ((bitboard >> 7 & game.not_afile) | (bitboard >> 9 & game.not_hfile)) & white_pieces;
}

int Board::bit_index(uint64_t bitboard) {
	const unsigned long long debruijin = 0x03f79d71b4cb0a89;
	return game.index64[(bitboard * debruijin) >> 58];
}

void Board::print_board() {
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			char current_piece_display;
			int shift = x + 8 * (7 - y);

			if ((white_pieces >> shift) % 2)
				current_piece_display = 'W';
			else if ((black_pieces >> shift) % 2)
				current_piece_display = 'B';
			else
				current_piece_display = '-';

			cout << current_piece_display << " ";
		}

		cout << "\n";
	}

	cout << "\n";

	cout << "Zobrist: " << zobrist;
	cout << "\n";
}

uint64_t Board::get_white_hash() const {
	if (!(white_pieces & 0x00ffff0000000000))
		return -1;

	uint64_t wpieces = white_pieces & 0x00ffff0000000000;
	uint64_t bpieces = black_pieces & 0xffff000000000000;
	uint64_t hash = 0;

	while (wpieces) {
		uint64_t value = bit_index(wpieces & (~wpieces + 1)) - 40;
 		hash +=  2LL * int_pow(3, value);
		wpieces = wpieces & (wpieces - 1);
	}

	while (bpieces) {
		uint64_t value = bit_index(bpieces & (~bpieces + 1)) - 40;
		hash += int_pow(3, value);
		bpieces = bpieces & (bpieces - 1);
	}

	return hash;
}

uint64_t Board::get_black_hash() const {
	if (!(black_pieces & 0xffff00))
		return -1;

	uint64_t wpieces = white_pieces & 0x000000000000ffff;
	uint64_t bpieces = black_pieces & 0x0000000000ffff00;
	uint64_t hash = 0;

	while (wpieces) {
		uint64_t value = bit_index(wpieces & (~wpieces + 1));
		hash +=  int_pow(3, 24-value-1);
		wpieces = wpieces & (wpieces - 1);
	}

	while (bpieces) {
		uint64_t value = bit_index(bpieces & (~bpieces + 1));
		hash += 2LL * int_pow(3, 24-value-1);
		bpieces = bpieces & (bpieces - 1);
	}

	return hash;
}


uint64_t Board::int_pow(int num, int exp) const {
	uint64_t result = 1;
	uint64_t base = num;
	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}

float Board::get_result_no_tables() const {
	int perspective = side == 0 ? 1 : -1;

	if (white_pieces & 0xff00000000000000LL) {
		return 1000 * perspective;
	}
	 
	if (black_pieces & 0xffLL) {
		return -1000 * perspective;
	}

	return -1;
}

float Board::get_result(int depth) const {
	int perspective = side == 0 ? 1 : -1;

	if (white_pieces & 0xff00000000000000LL) {
		return (1000+depth) * perspective;
	}

	if (black_pieces & 0xffLL) {
		return (-1000-depth) * perspective;
	}

	int white_eval = game.tablebase.get_eval(get_white_hash());
	int black_eval = game.tablebase.get_eval(get_black_hash());

	if (white_eval > 0 && white_eval >= black_eval && moves % 2 == 0) {
		return (1000 - white_eval * 2 + depth) * perspective;
	}

	if (black_eval > 0 && black_eval >= white_eval && moves % 2 == 1) {
		return (-1000 + black_eval * 2 - depth) * perspective;
	}
		
	return -1;
}

float Board::eval() const {
	if (side == 0) {
		return game.white_function(this, 0);
	}
	else {
		return game.black_function(this, 1);
	}
}

void Board::make_move(int start, int end) {
	if (moves % 2 == 0) {
		white_pieces &= ~(1LL << start);
		white_pieces |= (1LL << end);

		zobrist ^= game.zobrist_piece_nums[start][0];
		zobrist ^= game.zobrist_piece_nums[end][0];

		if ((black_pieces >> end) & 1)
			zobrist ^= game.zobrist_piece_nums[end][1];

		black_pieces &= ~(1LL << end);
	}
	else {
		black_pieces &= ~(1LL << start);
		black_pieces |= (1LL << end);

		zobrist ^= game.zobrist_piece_nums[start][1];
		zobrist ^= game.zobrist_piece_nums[end][1];

		if ((white_pieces >> end) & 1)
			zobrist ^= game.zobrist_piece_nums[end][0];

		white_pieces &= ~(1LL << end);
	}

	zobrist ^= game.zobrist_side_to_move;
	moves++;
}

void Board::make_move(Move move) {
	make_move(move.start, move.end);
}


int Board::set_successors(Board *successors) {
	uint64_t pieces = moves % 2 == 0 ? white_pieces : black_pieces;
	uint64_t(Board::*get_moves)(uint64_t) = moves % 2 == 0 ? &Board::generate_white_moves : &Board::generate_black_moves;
	int num_moves = 0;

	while (pieces) {
		uint64_t piece = pieces & (~pieces + 1);
		uint64_t moves = (this->*get_moves)(piece);
		int start = bit_index(piece);

		while (moves) {
			uint64_t move = moves & (~moves + 1);
			int end = bit_index(move);
			successors[num_moves] = *this;
			successors[num_moves].make_move(start, end);

			num_moves++;
			moves = moves & (moves - 1);
		}

		pieces = pieces & (pieces - 1);
	}

	return num_moves;
}

bool Board::operator<(const Board& other) const {
	return eval() < other.eval();
}

bool Board::operator==(const Board & other) const {
	return zobrist == other.zobrist;
}