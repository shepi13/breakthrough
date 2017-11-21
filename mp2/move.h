#pragma once
#include <cstdint>
#include <iostream>
#include <string>

class Move {
public:
	int start;
	int end;
	bool capture;

	void set(std::string notation);
	bool is_legal();

	std::string to_string() const;
	friend std::ostream& operator<<(std::ostream &os, const Move& move);
};