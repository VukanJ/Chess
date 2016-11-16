#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include <stack>
#include <chrono>

#include "Data.h"
#include "ZobristHash.h"
#include "misc.h"

using namespace std;

struct Move
{
	Move() : from(0), to(0), flags(0), Pieces(nulPiece) {}
	Move(byte OldCastlingRights, byte _flags)
		: from(OldCastlingRights), to(nulSq), flags(_flags), Pieces(nulPiece) {}
	Move(byte _from, byte _to, byte _flags, byte _pieces)
		: from(_from), to(_to), flags(_flags), Pieces(_pieces) {}
	byte from, to, flags, Pieces;
	// Flagbits: 1-4: castlingRuleReset?[k,K,w,W]; 5-8: Movetype
};

class Board
{
public:
	Board();
	Board(string fen);
	// Init
	void initHash(); // Used only for Init!, this key is updated for each move
	void debug();

	// Move making
	void updateAttack(piece p);
	void updateAllAttacks();
	void generateMoveList(vector<Move>&, color) const;
	void makeMove(const Move&, color side);
	void unMakeMove(const Move&, color side);
	bool isCheckMate(color) const;

	// Evaluation
	float evaluate();
	inline unsigned blockedPawn(color);

	// Flooding algorithm
	enum dir { n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);

	// Misc
	void print() const;

	// Data
	vector<u64> pieces, attacks;
	enum{ Ck = 0x1, CCk = 0x2, CK = 0x4, CCK = 0x8 };
	byte castlingRights, b_enpassent, w_enpassent;
	u64 whitePos, blackPos, allPos, whiteAtt, blackAtt,hashKey;
	Zob_Hash hash;
	vector<vector<u64>> randomSet;
	color sideToMove; // To be implemented
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE){
		return "b o-o";
	}
	else if (m.flags == WCASTLE){
		return "w o-o";
	}
	else if (m.flags == PROMOTION){
		return TARGET_PIECE(m.Pieces) == bq || TARGET_PIECE(m.Pieces) == wq ? "Queen prom" : "Knight prom";
	}
	else if (m.flags == C_PROMOTION){
		return TARGET_PIECE(m.Pieces) == bq || TARGET_PIECE(m.Pieces) == wq ? "Queen c_prom" : "Knight c_prom";
	}
	else if (m.flags == BCASTLE_2){
		return "b O--O";
	}
	else if (m.flags == WCASTLE_2){
		return "w O--O";
	}
	string s(1, names[MOV_PIECE(m.Pieces)]);
	if (m.flags == CAPTURE){
		if (s[0] == 'P'){
			s.clear();
			s += 'h' - (m.to % 8);
		}
		s += 'x';
	}
	if (s[0] == 'P')s.clear();
	s += 'h'-(m.to % 8);
	s += '1'+(m.to / 8);
	return s;
}
#endif
