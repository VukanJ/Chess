#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include <iostream>
#include <random>

#include "misc.h"
#include "Move.h"

// ZobristHash stores already evaluated Boards and their value,
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
		unsigned char search_depth, age;
		//Move bestMove;
		U8 flags : 3, terminal : 1, d : 3;
	};
	vector<entry> entries; // Hash table
	size_t hashSize;
public:
	ZobristHash();
	ZobristHash(size_t hashSize);
	entry& getEntry(const U64& key);
	int getValue(const U64& key) const;
	bool isRepetition(const U64& key, int depth) const;
	void clear();
};

class PVTable
{
private:
	struct PVEntry 
	{
		Move bestmove;
	};
	vector<PVEntry> pventries;
	size_t hashSize;
public:
	PVTable();
	PVTable(size_t hashSize);
	void addPVMove(const U64& key, const Move& best);
	PVEntry& operator[](const U64& key);
	void clear();
};

#endif
