#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

using namespace std;

#ifndef __MACHINEX64
#pragma message ("X64 RECOMMENDED!\n COMPILATION MAY FAIL")
#endif
#ifdef _WIN32
  typedef unsigned __int64 u64;
  typedef unsigned __int16 u16;
  #define POPCOUNT(x) __popcnt64(x)
  #define BITSCANR64(index,mask) _BitScanReverse64(&index,mask)
  #define ROTL64(mask, amount) _rotl64(mask,amount)
  #define ROTR64(mask, amount) _rotr64(mask,amount)
#elif __linux__
typedef unsigned long long u64;
#endif

#define BITLOOP(pos,mask) for (unsigned long pos = BITSCANR64(pos, mask); BITSCANR64(pos,mask) ; mask ^= (u64)1 << pos)

const u64 _start = (u64)1 << 63;
const u64 _col = 0x101010101010101;
const u64 _row = 0xFF;

static void printBitboard(u64 board)
{
	int c = 0;
	for (u64 p = (u64)1 << 63; p; p >>= 1){
		cout << (p & board ? '@' : '.');
		if (++c % 8 == 0) cout << endl;
	}
	cout << endl;
}

static void printBitboardFigAttack(u64 board, u64 att, char piece)
{
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
	wp, wr, wn, wb, wk, wq
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

const string names = "prnbkqPRNBKQ";

enum {
	h1, g1, f1, e1, d1, c1, b1, a1,
	h2, g2, f2, e2, d2, c2, b2, a2,
	h3, g3, f3, e3, d3, c3, b3, a3,
	h4, g4, f4, e4, d4, c4, b4, a4,
	h5, g5, f5, e5, d5, c5, b5, a5,
	h6, g6, f6, e6, d6, c6, b6, a6,
	h7, g7, f7, e7, d7, c7, b7, a7,
	h8, g8, f8, e8, d8, c8, b8, a8,
};

#endif 