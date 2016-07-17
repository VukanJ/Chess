#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include <stack>
#include <list>
#include <chrono>

#include "Data.h"
#include "ZobristHash.h"
#include "misc.h"

using namespace std;

struct Move
{
	Move() : from(0), to(0), flags(0), p((piece)-1), target((piece)-1){}
	Move(byte _from, byte _to, byte _flags, piece _p, piece _target)
		: from(_from), to(_to), flags(_flags), p(_p), target(_target){}
	
	byte from, to, flags;
	// Flagbits: 1-4: castlingRuleReset?[k,K,w,W]; 5-8: Movetype
	piece p, target;
};

class Board
{
public:
	Board();
	Board(string fen);
	// Init
	void initHash(); // Used only for Init!, this key is updated for each move
	
	// Move making
	void updateAttack(piece p);
	void updateAllAttacks();
	void generateMoveList(list<Move>&, color) const;
	void makeMove(const Move&);
	void unMakeMove(const Move&);
	bool isCheckMate(color) const;

	// Flooding algorithm
	enum dir { n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);

	// Misc
	void print();

	vector<u64> pieces, attacks;
	enum{
		Ck = 0x1, CCk = 0x2, CK = 0x4, CCK = 0x8
	};
	byte castlingRights;
	byte b_enpassent, w_enpassent;
	u64 whitePos, blackPos, whiteAtt, blackAtt, hashKey;
	Zob_Hash hash;
	vector<vector<u64>> randomSet;
	stack<byte> moveHistory; // stores additional information about castling, enpassent
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE){
		return "b rochade";
	}
	else if (m.flags == WCASTLE){
		return "w rochade";
	}
	else if (m.flags == PROMOTION){
		return m.target == bq || m.target == wq ? "Queen prom" : "Knight prom";
	}
	else if (m.flags == C_PROMOTION){
		return m.target == bq || m.target == wq ? "Queen c_prom" : "Knight c_prom";
	}
	else if (m.flags == BCASTLE_2){
		return "b grand rochade";
	}
	else if (m.flags == WCASTLE_2){
		return "w grand rochade";
	}
	string s(1, names[m.p%6+6]);
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