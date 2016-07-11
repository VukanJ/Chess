#include "Board.h"

Board::Board()
 : whitePos(0), blackPos(0), whiteAtt(0), blackAtt(0)
{
	pieces = vector<u64>(12, 0x0);
	attacks = vector<u64>(12, 0x0);
}

Board::Board(string fen)
{
	if (fen == "*"){
		pieces = vector<u64>(12, 0x0);
		attacks = vector<u64>(12, 0x0);
		whitePos = blackPos = whiteAtt = blackAtt = 0x0;
		pieces[bp] = (u64)0xFF << 8;
		pieces[br] = (u64)0x81;
		pieces[bn] = (u64)0x42;
		pieces[bb] = (u64)0x24;
		pieces[bk] = (u64)0x10;
		pieces[bq] = (u64)0x8;
		pieces[wp] = (u64)0xFF << 48;
		pieces[wr] = (u64)0x81 << 56;
		pieces[wn] = (u64)0x42 << 56;
		pieces[wb] = (u64)0x24 << 56;
		pieces[wk] = (u64)0x10 << 56;
		pieces[wq] = (u64)0x8 << 56;
		blackPos = 0xFFFF;
		whitePos = 0xFFFF00000000;
	}
	else {
		pieces = vector<u64>(12, 0x0);
		attacks = vector<u64>(12, 0x0);
		whitePos = blackPos = whiteAtt = blackAtt = 0x0;
		int counter = -1;
		for (auto& p : fen) {
			if (isdigit(p)) {
				counter += p - 48;
			}
			else {
				counter++;
				switch (p) {
					case 'p': pieces[bp] |= (u64)1 << (63 - counter); break;
					case 'r': pieces[br] |= (u64)1 << (63 - counter); break;
					case 'n': pieces[bn] |= (u64)1 << (63 - counter); break;
					case 'b': pieces[bb] |= (u64)1 << (63 - counter); break;
					case 'k': pieces[bk] |= (u64)1 << (63 - counter); break;
					case 'q': pieces[bq] |= (u64)1 << (63 - counter); break;
					case 'P': pieces[wp] |= (u64)1 << (63 - counter); break;
					case 'R': pieces[wr] |= (u64)1 << (63 - counter); break;
					case 'N': pieces[wn] |= (u64)1 << (63 - counter); break;
					case 'B': pieces[wb] |= (u64)1 << (63 - counter); break;
					case 'K': pieces[wk] |= (u64)1 << (63 - counter); break;
					case 'Q': pieces[wq] |= (u64)1 << (63 - counter); break;
					case '/': break;
					default: cerr << "Bad FEN! (Board::Board())\n"; exit(1); break;
				}
			}
		}
		for (int p = 0; p < 6; p++)
			blackPos |= pieces[p];
		for (int p = 6; p < 12; p++)
			whitePos |= pieces[p];
		for (int i = 0; i < 12; ++i)
			updateAttacks((piece)i);
	}
}

void Board::updateAttacks(piece p)
{
	// Fill all the bits that are attacked by individual pieces
	// including attacked enemy pieces
	// Patterns: 
	unsigned long pos = -1;
	u64 mask = 0;
	switch (p){
		case bp: break;
		case br: break;
		case bn: 
			mask = pieces[bn];
			attacks[bn] = 0x0;
			BITLOOP(pos,mask) attacks[bn] |= KNIGHT_ATTACKS[pos] & ~blackPos;
			break;
		case bb: break;
		case bk: break;
		case bq: break;

		case wp: break;
		case wr: break;
		case wn:
			mask = pieces[wn];
			attacks[wn] = 0x0;
			BITLOOP(pos, mask) attacks[wn] |= KNIGHT_ATTACKS[pos] & ~whitePos;
			break;
		case wb: break;
		case wk: break;
		case wq: break;
	}
	// Compute combined attacks:
	blackAtt = whiteAtt = 0;
	for (int i = 0; i < 6; ++i)
		blackAtt |= attacks[i];
	for (int i = 6; i < 12; ++i)
		whiteAtt |= attacks[i];

	// King does not really attack fields, that are under attack
	// These moves are not considered by the move-generator:
	//attacks[bk] ^= (attacks[bk] & whiteAtt);
	//attacks[wk] ^= (attacks[wk] & blackAtt);
}

void Board::print()
{
	static vector<string> asciiBoard = vector<string>(8, string(8, ' '));
	for (auto& r : asciiBoard)
		for (auto& c : r)
			c = '.';
	for (int p = 0; p < 12; p++) {
		auto temp = pieces[p];
		auto count = -1;
		for (u64 b = _start; b != 0; b >>= 1) {
			count++;
			if (b & temp)
				asciiBoard[count / 8][count % 8] = names[p];
		}
	}
	cout << string(10, 220) << endl;
	for (auto r : asciiBoard) {
		cout << (char)219;
		for (auto c : r)
			cout << c;
		cout << (char)219 << endl;
	}
	cout << string(10, 223) << endl;
}