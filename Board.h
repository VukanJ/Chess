#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <string>
#include <stack>
#include <chrono>
#include <boost/algorithm/string.hpp>

#include "Move.h"
#include "Data.h"
#include "ZobristHash.h"

using namespace std;

class Board
{
public:
	Board();
	Board(string fen);
	// Init
	void initHash(); // Used only for Init!, this key is updated for each move
	void debug();
	void setupBoard(string FEN);

	// Move making
	void updateAttack(piece p);
	void updateAllAttacks();
	void generateMoveList(vector<Move>&, color) const;
	bool makeMove(const Move&, color side);
	void unMakeMove(const Move&, color side);
	bool isCheckMate(color) const;

	// Evaluation
	int evaluate(color side);
	inline unsigned blockedPawn(color);

	// Flooding algorithm
	enum dir { n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);

	// Misc
	void print() const;

	// Data
	vector<u64> pieces, attacks;
	enum{ castle_k = 0x1, castle_q = 0x2, castle_K = 0x4, castle_Q = 0x8 };
	byte castlingRights, b_enpassent, w_enpassent;
	u64 wpMove, bpMove, whitePos, blackPos, allPos, whiteAtt, blackAtt, hashKey;
	Zob_Hash hash;
	vector<vector<u64>> randomSet;
	color sideToMove; // To be implemented
};

#endif
