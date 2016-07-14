#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include <list>
#include <chrono>

#include "Data.h"
#include "ZobristHash.h"
#include "misc.h"

using namespace std;

const u32 fromBits = (u32)0x3F;     
const u32 toBits   = (u32)0xFC0;    
const u32 pType    = (u32)0xF000;   
const u32 mType    = (u32)0xF0000;
const u32 cPiece   = (u32)0xF00000; 

struct Move
{
	Move() : from(0), to(0), flags(0), p((piece)-1), target((piece)-1){}
	Move(byte _from, byte _to, byte _flags, piece _p, piece _target)
		: from(_from), to(_to), flags(_flags), p(_p), target(_target){}
	
	byte from, to, flags;
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
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE){
		return "rochade";
	}
	else if (m.flags == PROMOTION){
		return m.target == bq ? "Queen prom" : "Knight prom";
	}
	else if (m.flags == C_PROMOTION){
		return m.target == bq ? "Queen c_prom" : "Knight c_prom";
	}
	else if (m.flags == BCASTLE_2){
		return "grand rochade";
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