#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

#include "Data.h"
#include "misc.h"

using namespace std;

class Board
{
public:
	Board();
	Board(string fen);
	vector<u64> pieces, attacks;
	u64 whitePos, blackPos, whiteAtt, blackAtt;
	void updateAttacks(piece p);
	void print();
};

#endif 