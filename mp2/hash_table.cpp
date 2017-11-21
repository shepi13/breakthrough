#include "hash_table.h"
#include "move.h"
#include "game.h"
#include <new>
#include <iostream>

HashTable::HashTable() {
	size = HASHTABLE_SIZE / sizeof(HashEntry);
	table = new HashEntry[size];
	reset();
}

HashTable::~HashTable() {
	delete table;
}

void HashTable::reset() {
	HashEntry zero_element;
	std::fill(table, table + size, zero_element);
}

void HashTable::put(HashEntry entry) {
	uint64_t idx = entry.key % size;

	if (entry.depth >= table[idx].depth) {
		table[idx].key = entry. key;
		table[idx].eval = entry.eval;
		table[idx].depth = entry.depth;
		table[idx].type = entry.type;
	 }
}

HashEntry HashTable::get(uint64_t key) {
	uint64_t idx = key % size;

	if (table[idx].key != key) {
		game.table_misses++;
		return HashEntry(0, 0, 0, MISS);
	}

	game.table_hits++;
	return table[idx];
}