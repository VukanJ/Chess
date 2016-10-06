#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

using namespace std;

#ifndef __MACHINEX64
#pragma message ("X64 ARCHITECTURE RECOMMENDED!\n COMPILATION MAY FAIL")
#endif
#ifdef _WIN32
  typedef unsigned __int64 u64;
  typedef unsigned __int32 u32;
  typedef unsigned __int16 u16;
  typedef unsigned __int8  byte;
  #define POPCOUNT(x) __popcnt64(x)                             // Population count
  #define BITSCANR64(index,mask) _BitScanReverse64(&index,mask) // Reverse Bitscan
  #define ROTL64(mask, amount) _rotl64(mask,amount)             // Rotate left (64Bit)
  #define ROTR64(mask, amount) _rotr64(mask,amount)             // Rotate right (64Bit)
  #define BIT_AT(x) (0x1ull << x)                               // Sets nth bit in number counting from least significant bit
  #define BIT_AT_R(x) (0x1ull << (63-x))                        // Sets nth bit in number counting from most significant bit
  #define PIECE_PAIR(X,Y) (X | (Y << 4))                        // Pairs up 4-bit piece information 
  // Move formatting macros:
  #define MOV_PIECE(DATA) (DATA & 0xF)                          // Piece that moves 
  #define TARGET_PIECE(DATA) ((DATA & 0xF0) >> 4)               // Target piece
  #define MOVE_TYPE(DATA) ((DATA & (0xFFull << 8))>>8)          // Move type
  #define MOVE_TO(DATA) ((DATA & (0xFFull << 16))>>16)          // To-Square
  #define MOVE_FROM(DATA) ((DATA & (0xFFull << 24))>>24)        // From-Square
#elif __linux__
typedef unsigned long long u64;
#endif

#define BITLOOP(pos,mask) for (unsigned long pos = BITSCANR64(pos, (mask)); BITSCANR64(pos, (mask)) ; (mask) ^= 1ull << pos)
#define BLACKLOOP(x) for (int x = 0; x < 6;  ++x)
#define WHITELOOP(x) for (int x = 6; x < 12; ++x)

const u64 _msb       = 0x11ULL << 63;
const u64 _col       = 0x101010101010101ull;
const u64 _row       = 0xFFull;
const u64 _noSides   = 0x7E7E7E7E7E7E7E7Eull;
const u64 _sidesOnly = 0x8181818181818181ull;

const string names = "prnbkqPRNBKQ";

static void printBits(byte num)
{
	int c = 0;
	for (byte p = 0x1 << 7; p; p >>= 1)
		cout << (p & num ? '1' : '0');
	cout << endl;
}

static void printBitboard(u64 board)
{
	// Prints Bitboard and occupied pieces
	int c = 0;
	for (u64 p = (u64)1 << 63; p; p >>= 1){
		cout << (p & board ? '@' : '.');
		if (++c % 8 == 0) cout << endl;
	}
	cout << endl;
}

static void printBitboardFigAttack(u64 board, u64 att, char piece)
{
	// Print Bitboard for specific figure and attackes squares
	int c = 0;
	for (u64 p = (u64)1 << 63; p; p >>= 1){
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
	MOVE, 
	CAPTURE, 
	PAWN2, 
	PROMOTION,
	C_PROMOTION, // Capture and promotion
	ENPASSENT, 
	WCASTLE, 
	WCASTLE_2, 
	BCASTLE, 
	BCASTLE_2,
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

enum hashPosition{CASTLE_HASH=12,ENPASSENT_HASH=13};

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