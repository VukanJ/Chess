#include "Board.h"

Board::Board()
{
	pieces = vector<u64>(12, 0);
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
	whitePos = blackPos = 0;
	for (int p = 0; p < 6; p++)
		blackPos |= pieces[p];
	for (int p = 6; p < 12; p++)
		whitePos |= pieces[p];
	print();
}

Board::Board(string fen)
{
	int counter = 0;
	for (auto& p : fen) {
		if (isdigit(p)) {
			counter += p + 48;
		}
		else {
			counter++;
			switch (p) {
				case 'p': pieces[bp] |= (u64)1<<counter; break;
				case 'r': pieces[br] |= (u64)1<<counter; break;
				case 'n': pieces[bn] |= (u64)1<<counter; break;
				case 'b': pieces[bb] |= (u64)1<<counter; break;
				case 'k': pieces[bk] |= (u64)1<<counter; break;
				case 'q': pieces[bq] |= (u64)1<<counter; break;
				case 'P': pieces[wp] |= (u64)1<<counter; break;
				case 'R': pieces[wr] |= (u64)1<<counter; break;
				case 'N': pieces[wn] |= (u64)1<<counter; break;
				case 'B': pieces[wb] |= (u64)1<<counter; break;
				case 'K': pieces[wk] |= (u64)1<<counter; break;
				case 'Q': pieces[wq] |= (u64)1<<counter; break;
				case '/': break;
				default: cerr << "Bad FEN! (Board::Board())\n"; exit(1); break;
			}
		}
	}
}

void Board::updateAttacks(piece p)
{
	unsigned long pos = -1;
	switch (p){
		case bp: ;break;
		case br: ;break;
		case bn: ;break;
		case bb: ;break;
		case bk: 
			_BitScanReverse(&pos, pieces[bk]);
			attacks[bk] = whitePos ^ ((u64)0x70507 << ( pos - 9)); 
			break;
		case bq: ;break;

		case wp: ;break;
		case wr: ;break;
		case wn: ;break;
		case wb: ;break;
		case wk: ;break;
		case wq: ;break;
	}
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