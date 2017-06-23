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
	Move(U8 OldCastlingRights, U8 _flags);
	Move(U8 _from, U8 _to, U8 _flags, U8 _pieces);

	Move& operator=(const Move&);

	U8 from, to, flags, pieces;
	U8 inline movePiece()   const { return pieces & 0xF; }
	U8 inline targetPiece() const { return pieces >> 4; }
	U8 inline mtype()       const { return flags & 0xF; }
	U8 inline castlingRights() const { return flags >> 4; }
	U8 inline oldCastlingRights() const { return from; }
	// Flagbits: 1-4: castlingRuleReset?[k,K,w,W]; 5-8: Movetype
};

static string moveString(Move m)
{
	// Short algebraic notation (AN)
	if (m.flags == BCASTLE || m.flags == WCASTLE) {
		return "O-O";
	}
	else if (m.flags == PROMOTION) {
		return string(1, squareNames[m.to][0]) + "8=" + string(1, toupper(names[m.targetPiece()]));
	}
	else if (m.flags == C_PROMOTION) {
		return string(1, squareNames[m.to][0]) + "x" + squareNames[m.to] + "=" + string(1, toupper(names[m.targetPiece()]));
	}
	else if (m.flags == BCASTLE_2 || m.flags == WCASTLE_2) {
		return "O-O-O";
	}
	string s(1, toupper(names[m.movePiece()]));
	if (m.flags == CAPTURE) {
		if (s[0] == 'P')
			s = squareNames[m.to][0];
		s += 'x';
	}
	if (s[0] == 'P')s.clear(); // P is left out
	s = squareNames[m.to];
	return s;
}

static string shortNotation(const Move& move)
{
	// returns string with mainly from to square information
	if (move.flags == BCASTLE || move.flags == WCASTLE) {
		return "O-O";
	}
	if (move.flags == BCASTLE_2 || move.flags == WCASTLE_2) {
		return "O-O-O";
	}
	string moveStr = string(1,(char)('h' - (move.from % 8)))
		           + string(1,(char)('1' + (move.from / 8)))
		           + string(1,(char)('h' - (move.to % 8)))
		           + string(1,(char)('1' + (move.to / 8)));
	if (move.flags == PROMOTION || move.flags == C_PROMOTION) {
		moveStr += names[move.targetPiece()];
	}
	return moveStr;
}

#endif
