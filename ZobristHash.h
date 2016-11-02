#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <iostream>
#include <random>
#include "misc.h"

// Zob_Hash stores already evaluated Boards
// by Zobrist-key

class Zob_Hash
{
public:
	Zob_Hash();
	void addEntry(const u64 Key, float value, int depth);
	bool getEntry(const u64 Key, float& value, int& depth) const;
	bool getEntry(const u64 key, float& value) const;
	bool hasEntry(const u64) const;
	vector<vector<u64>> getRandomSet(); // Used once for init
private:
	struct entry
	{
		entry() : value(0), search_depth(-1){}
		float value;
		int search_depth;
	};
	vector<entry> entries;
};

#endif