#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <string>
#include <chrono>
#include <boost/algorithm/string.hpp>

#include "Move.h"
#include "Data.h"
#include "ZobristHash.h"

using namespace std;

class Board
{
private:
	// Move making
	void updateAttack(piece p);
	
	// Init
	void initHash(); // Used only for Init!, the hashkey is updated for each move
	
	// Evaluation
	inline unsigned blockedPawn(color);

	// Flooding algorithm
	enum dir {n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);
public:
	Board();
	Board(string fen);
	// Init
	void debug();
	void setupBoard(string FEN);

	// Move making
	void generateMoveList(vector<Move>&, color) const;
	void makeMove(const Move&, color side);
	void unMakeMove(const Move&, color side);
	void updateAllAttacks();

	// Evaluation
	int evaluate(color side, int depth);

	// Misc
	void print() const;

	// Raw data: public for easy access
	vector<u64> pieces, attacks;
	enum castlingRight{ castle_k = 0x1, castle_q = 0x2, castle_K = 0x4, castle_Q = 0x8 };
	byte castlingRights, b_enpassent, w_enpassent;
	u64 wpMove, bpMove, whitePos, blackPos, allPos, whiteAtt, blackAtt, hashKey;
	ZobristHash hash;
	vector<vector<u64>> randomSet;
};

#endif
