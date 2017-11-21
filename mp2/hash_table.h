#pragma once
#include <cstdint>
#include "hash_entry.h"
#include "move.h"

#define HASHTABLE_SIZE (524288000LL*4LL)

class HashTable
{
private:
	HashEntry* table;
	uint64_t size;

public:
	HashTable();
	~HashTable();

	void reset();
	HashEntry get(uint64_t key);
	void put(HashEntry entry);
};