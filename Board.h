#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include <list>

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
	vector<u64> pieces, attacks;
	u64 whitePos, blackPos, whiteAtt, blackAtt, hashKey;
	void updateAttack(piece p);
	void updateAllAttacks();
	void initHash(); // Used only for Init!, this key is updated for each move
	
	void generateMoveList(list<Move>&, color) const;
	void makeMove(const Move&);
	void unMakeMove(const Move&);
	void print();

	// Flooding algorithm
	enum dir { n, e, s, w, ne, se, sw, nw};
	u64 floodFill(u64 propagator, u64 empty, dir);
	void pawnFill(color side);

	Zob_Hash hash;
	vector<vector<u64>> randomSet;
};

static string moveString(Move m)
{
	string s(1, names[m.p]);
	s += 'h'-(m.to % 8);
	s += '1'+(m.to / 8);
	if (m.flags == CAPTURE)
		s += '!';
	return s;
}
#endif 