#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <iostream>
#include <random>

#include "misc.h"
#include "Move.h"

// Zob_Hash stores already evaluated Boards and their value, 
// search_depth and move quality

enum hashPosition { CASTLE_HASH = 12, ENPASSENT_HASH = 13, CASTLE_POSITION_HASH = 14};
enum hashDoCastle { HASH_CASTLE_k, HASH_CASTLE_q, HASH_CASTLE_K, HASH_CASTLE_Q};

enum valueType {
	EXACT_VALUE = 0x1, // Board value lies between alpha and beta
	LOWER_BOUND = 0x2, // Board value was < alpha
	UPPER_BOUND = 0x4  // Board value was > beta
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
		Move move;
		byte flags;
	};
	vector<entry> entries; // Hash table
	size_t hashSize;
public:
	ZobristHash();
	ZobristHash(size_t hashSize);
	entry* const addEntry(const u64 Key, int  value, int depth);
	entry& getEntry(const u64 Key);
	int getValue(const u64 key) const;

	void inline setBoundFlags(const u64 key, valueType);
};

#endif