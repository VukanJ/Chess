#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

#include "defines.h"

using namespace std;

// TODO: Check if variable for all pieces is necessarry
// TODO: Update Positions after move

const u64 _msb       = 0x11ull << 63;
const u64 _col       = 0x101010101010101ull;
const u64 _row       = 0xFFull;
const u64 _noSides   = 0x7E7E7E7E7E7E7E7Eull;
const u64 _sidesOnly = 0x8181818181818181ull;

const string names = "prnbkqPRNBKQ";

static void printBits(byte num)
{
	int c = 0;
	for (byte p = 0x1ull << 7; p ; p >>= 1)
		cout << (p & num ? '1' : '0');
	cout << endl;
}

static void printBitboard(u64 board)
{
	// Prints Bitboard and occupied pieces
	int c = 0;
	for (u64 p = 1ull << 63; p; p >>= 1){
		cout << (p & board ? '@' : '.');
		if (++c % 8 == 0) cout << endl;
	}
	cout << endl;
}

static void printBitboardFigAttack(u64 board, u64 att, char piece)
{
	// Print Bitboard for specific figure and attackes squares
	int c = 0;
	for (u64 p = 1ull << 63; p; p >>= 1){
		if (board & p) cout << piece;
		else if (att & p) cout << '#';
		else cout << '.';
		if (++c % 8 == 0) cout << endl;
	}
	cout << endl;
	if (board&att) cout << "Warning: Attacking same color!\n"; 
}

enum piece {
	bp, br, bn, bb, bk, bq,
	wp, wr, wn, wb, wk, wq,
	nulPiece = -1
};

enum color { black, white };

enum moveType{ 
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

static piece getPieceIndex(char p)
{
	piece res;
	switch (p){
		case 'p':res = bp; break;
		case 'r':res = br; break;
		case 'n':res = bn; break;
		case 'b':res = bb; break;
		case 'k':res = bk; break;
		case 'q':res = bq; break;
		case 'P':res = wp; break;
		case 'R':res = wr; break;
		case 'N':res = wn; break;
		case 'B':res = wb; break;
		case 'K':res = wk; break;
		case 'Q':res = wq; break;
		default: throw;
	}
	return res;
}

enum attacks {
	att_bp, att_br, att_bn, att_bb, att_bk, att_bq,
	att_wp, att_wr, att_wn, att_wb, att_wk, att_wq
};

enum hashPosition{CASTLE_HASH = 12, ENPASSENT_HASH = 13};

enum {
	h1, g1, f1, e1, d1, c1, b1, a1,
	h2, g2, f2, e2, d2, c2, b2, a2,
	h3, g3, f3, e3, d3, c3, b3, a3,
	h4, g4, f4, e4, d4, c4, b4, a4,
	h5, g5, f5, e5, d5, c5, b5, a5,
	h6, g6, f6, e6, d6, c6, b6, a6,
	h7, g7, f7, e7, d7, c7, b7, a7,
	h8, g8, f8, e8, d8, c8, b8, a8,
	nulSq = -1
};

#endif 