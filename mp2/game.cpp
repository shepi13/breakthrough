#include "board.h"
#include "game.h"
#include <random>
#include <time.h>

using std::mt19937_64;

Game game;

Game::Game() : tablebase(3, 8), dist(0, 1) {
	reset();

	srand(time(NULL));
	mt19937_64 mt_rand(time(0));
	rng.seed(std::random_device{}());

	for (int i = 0; i < 64; i++) {
		zobrist_piece_nums[i][0] = mt_rand();
		zobrist_piece_nums[i][1] = mt_rand();
	}

	zobrist_side_to_move = mt_rand();

	for (int i = 0; i < 16; i++) {
		board.zobrist ^= zobrist_piece_nums[i][0];
		board.zobrist ^= zobrist_piece_nums[63 - i][1];
	}

	tablebase.read_from_database(TABLE_NAME);
}

void Game::reset() {
	board.white_pieces = 0x000000000000ffff;
	board.black_pieces = 0xffff000000000000;

	board.moves = 0;
	board.side = 0;

	table_hits = 0;
	white_nodes_expanded = 0;
	black_nodes_expanded = 0;
	quiescence_nodes = 0;

	board.zobrist = 0;

	for (int i = 0; i < 16; i++) {
		board.zobrist ^= zobrist_piece_nums[i][0];
		board.zobrist ^= zobrist_piece_nums[63 - i][1];
	}

	transposition_table.reset();
}

int Game::result() {
	if (board.black_pieces & 0xffLL || board.white_pieces == 0)
		return -1;

	if (board.white_pieces & 0xff00000000000000 || board.black_pieces == 0)
		return 1;

	return 0;
}

void Game::make_move(int start, int end) {
	prev_positions.push_back(game.board);
	board.side = 1 - board.side;

	Move move;
	uint64_t enemies = board.moves == 0 ? board.black_pieces : board.white_pieces;
	move.start = start;
	move.end = end;
	move.capture = enemies & (1LL << move.end);

	if (board.moves % 2 == 0)
		score += std::to_string(board.moves / 2 + 1) + ". ";

	score += move.to_string() + " ";
	board.make_move(start, end);
}

void Game::make_move(Move move) {
	make_move(move.start, move.end);
}

void Game::undo_move() {
	board = prev_positions.back();
	prev_positions.pop_back();
}

Move Game::get_best_move() {
	uint64_t start_pieces = board.moves % 2 == 0 ? board.white_pieces : board.black_pieces;
	uint64_t end_pieces = board.moves % 2 == 0 ? best_successor.white_pieces : best_successor.black_pieces;
	uint64_t enemies = board.moves % 2 == 0 ? board.black_pieces : board.white_pieces;

	Move move;
	move.start = Board::bit_index((start_pieces ^ end_pieces) & ~end_pieces);
	move.end = Board::bit_index((start_pieces ^ end_pieces) & ~start_pieces);
	move.capture = ((1LL << move.end) & enemies);

	return move;
}