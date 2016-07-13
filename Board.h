#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

#include "Data.h"
#include "ZobristHash.h"
#include "misc.h"

using namespace std;

class Board
{
public:
	Board();
	Board(string fen);
	vector<u64> pieces, attacks;
	u64 whitePos, blackPos, whiteAtt, blackAtt, hashKey;
	void updateAttack(piece p);
	void updateAllAttacks();
	void computeHash(); // Used only for Init!, this key is updated for each move
	void print();

	// Flooding algorithm
	enum dir { n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);
	Zob_Hash hash;
	vector<vector<u64>> randomSet;
};

#endif 