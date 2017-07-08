#ifndef MOVE_H
#define MOVE_H
#include <iostream>

#include "misc.h"

using namespace std;

enum makeMoveType { FULL = 0x7, HASH_POS = 0x4, HASH = 0x1, POS = 0x2 };

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
	bool inline operator==(const Move& m) {
		return (m.from == from)
			&& (m.to == to)
			&& (m.flags == flags)
			&& (m.pieces == pieces);
	}

	U8 from, to, flags, pieces;
	bool inline invalid() const { return from == 255; }
	U8 inline movePiece() const { return pieces & 0xF; }
	U8 inline targetPiece() const { return pieces >> 4; }
	U8 inline mtype() const { return flags & 0xF; }
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
	string s(1, names[m.movePiece()]);
	if (m.flags == CAPTURE) {
		s += 'x';
		s += string("{") + names[m.targetPiece()] + "}";
	}
	s += squareNames[m.from] + squareNames[m.to];
	return s;
}

static string shortNotation(const Move& move)
{
	// returns string with mainly from to square information
	switch (move.mtype()) {
	case BCASTLE:   return "o-o";
	case WCASTLE:   return "O-O";
	case BCASTLE_2: return "o-o-o";
	case WCASTLE_2: return "O-O-O";
	case ENPASSENT: return "e.p.";
	}
	if (move.flags == 255) return "_voidMove_";
	string moveStr = squareNames[move.from] + squareNames[move.to];
	if (move.flags == PROMOTION || move.flags == C_PROMOTION) {
		moveStr += names[move.targetPiece()];
	}
	return moveStr;
}

static ostream& operator<<(ostream& stream, const Move& move)
{
	cout << shortNotation(move);
	return stream;
}


#endif
