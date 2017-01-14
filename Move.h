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
	Move(ulong _from, ulong _to, byte _flags, byte _pieces);
	Move(int _from, int _to, byte _flags, byte _pieces);
	byte from, to, flags, Pieces;
	// Flagbits: 1-4: castlingRuleReset?[k,K,w,W]; 5-8: Movetype
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE || m.flags == WCASTLE) {
		return "O-O";
	}
	else if (m.flags == PROMOTION) {
		return string(1, squareNames[m.to][0]) + "8=" + string(1, toupper(names[target_piece(m.Pieces)]));
	}
	else if (m.flags == C_PROMOTION) {
		return string(1, squareNames[m.to][0]) + "x" + squareNames[m.to] + "=" + string(1, toupper(names[target_piece(m.Pieces)]));
	}
	else if (m.flags == BCASTLE_2 || m.flags == BCASTLE_2) {
		return "O-O-O";
	}
	string s(1, toupper(names[move_piece(m.Pieces)]));
	if (m.flags == CAPTURE) {
		if (s[0] == 'P') 
			s = squareNames[m.to][0];
		s += 'x';
	}
	if (s[0] == 'P')s.clear(); // P is left out
	s = squareNames[m.to];
	return s;
}

#endif