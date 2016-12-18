#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <iostream>
#include <random>
#include "misc.h"

// Zob_Hash stores already evaluated Boards
// by Zobrist-key

enum hashPosition { CASTLE_HASH = 12, ENPASSENT_HASH = 13 };

class Zob_Hash
{
public:
	Zob_Hash();
	Zob_Hash(size_t hashSize);
	void addEntry(const u64 Key, int  value, int depth);
	bool getEntry(const u64 Key, int& value, int& depth) const;
	bool getEntry(const u64 key, int& value) const;
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