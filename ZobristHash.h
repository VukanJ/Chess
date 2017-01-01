#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <iostream>
#include <random>
#include "misc.h"

// Zob_Hash stores already evaluated Boards and their value, 
// search_depth and move quality

enum hashPosition { CASTLE_HASH = 12, ENPASSENT_HASH = 13};

enum MoveKind { 
	PV_MOVE = 0x1, // Principal variation (the move that should be played)
};

enum valueType {
	EXACT_VALUE = 0x100, // Board value lies between alpha and beta
	LOWER_BOUND = 0x200, // Board value was < alpha
	UPPER_BOUND = 0x400  // Board value was > beta
};

// Flag = [8 Bits: value type, 8 Bits: MoveKind]

class ZobristHash
{
private:
	struct entry
	{
		entry();
		int value;
		int search_depth;
		u16 flags;
	};
	vector<entry> entries; // Hash table
	u64 hashSize;
public:
	ZobristHash();
	ZobristHash(size_t hashSize);
	entry* const addEntry(const u64 Key, int  value, int depth);
	entry* const getEntry(const u64 Key, int& value, int& depth);
	entry* const getEntry(const u64 key, int& value);
	bool   hasEntry(const u64) const;
	entry* const hasBetterEntry(const u64 key, int depth);

	void inline setBoundFlags(const u64 key, valueType);
};

#endif