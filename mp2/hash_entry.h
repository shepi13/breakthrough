#pragma once
#include "board.h"

enum Type { EXACT, FAILLOW, FAILHIGH, MISS };

class HashEntry
{
public:
	Type type;
	float eval;
	int depth;
	uint64_t key;

	HashEntry() : type(MISS), eval(0), depth(0), key(0) {}
	HashEntry(uint64_t key, float eval, int depth, Type type) : type(type), eval(eval), depth(depth), key(key) {}
};