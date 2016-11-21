#ifndef MOVE_H
#define MOVE_H
#include <iostream>

#include "misc.h"

using namespace std;

enum moveType {
	MOVE,        // Quiet move
	CAPTURE,     // Capture
	PAWN2,       // Double pawn push
	PROMOTION,   // Pawn promotion
	C_PROMOTION, // Capture and promotion
	ENPASSENT,   // Enpassent
	WCASTLE,     // White castle o-o
	WCASTLE_2,   // White castle o-o-o
	BCASTLE,     // Black castle o-o
	BCASTLE_2,   // Black castle o-o-o
};

struct Move
{
	Move();
	Move(byte OldCastlingRights, byte _flags);
	Move(byte _from, byte _to, byte _flags, byte _pieces);
	Move(unsigned long _from, unsigned long _to, byte _flags, byte _pieces);
	Move(int _from, int _to, byte _flags, byte _pieces);
	byte from, to, flags, Pieces;
	// Flagbits: 1-4: castlingRuleReset?[k,K,w,W]; 5-8: Movetype
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE) {
		return "b-o-o";
	}
	else if (m.flags == WCASTLE) {
		return "w-o-o";
	}
	else if (m.flags == PROMOTION) {
		return TARGET_PIECE(m.Pieces) == bq || TARGET_PIECE(m.Pieces) == wq ? "Queen prom" : "Knight prom";
	}
	else if (m.flags == C_PROMOTION) {
		return TARGET_PIECE(m.Pieces) == bq || TARGET_PIECE(m.Pieces) == wq ? "Queen c_prom" : "Knight c_prom";
	}
	else if (m.flags == BCASTLE_2) {
		return "b-O--O";
	}
	else if (m.flags == WCASTLE_2) {
		return "w-O--O";
	}
	string s(1, names[MOV_PIECE(m.Pieces)]);
	if (m.flags == CAPTURE) {
		if (s[0] == 'P' || s[0] == 'p') {
			s.clear();
			s += 'h' - (m.to % 8);
		}
		s += 'x';
	}
	if (s[0] == 'P' || s[0] == 'p')s.clear();
	s += 'h' - (m.to % 8);
	s += '1' + (m.to / 8);
	return s;
}

#endif