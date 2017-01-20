#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

#include "defines.h"

using namespace std;

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
	// Do not change ordering!
	bp, br, bn, bb, bk, bq,
	wp, wr, wn, wb, wk, wq,
	nullPiece = -1
};

enum castlingRight { castle_k = 0x1, castle_q = 0x2, castle_K = 0x4, castle_Q = 0x8 };

enum color {black, white};

static piece getPieceIndex(char p)
{
	switch (p){
		case 'p': return bp;
		case 'r': return br;
		case 'n': return bn;
		case 'b': return bb;
		case 'k': return bk;
		case 'q': return bq;
		case 'P': return wp;
		case 'R': return wr;
		case 'N': return wn;
		case 'B': return wb;
		case 'K': return wk;
		case 'Q': return wq;
		default: throw;
	}
}

const vector<string> squareNames = 
{
	"h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
	"h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
	"h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
	"h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
	"h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
	"h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
	"h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
	"h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};

enum {
	h1, g1, f1, e1, d1, c1, b1, a1,
	h2, g2, f2, e2, d2, c2, b2, a2,
	h3, g3, f3, e3, d3, c3, b3, a3,
	h4, g4, f4, e4, d4, c4, b4, a4,
	h5, g5, f5, e5, d5, c5, b5, a5,
	h6, g6, f6, e6, d6, c6, b6, a6,
	h7, g7, f7, e7, d7, c7, b7, a7,
	h8, g8, f8, e8, d8, c8, b8, a8,
	nullSquare = -1
};

#endif 