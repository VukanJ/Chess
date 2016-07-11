#ifndef AI_H
#define AI_H
#include <iostream>
#include <string>

#include "Board.h"

using namespace std;

struct Move
{
	/* 
	 Bit 0-5:   From     (0-63)
	 Bit 6-11:  To       (0-63)
	*/
	u16 move;
};

class AI
{
public:
	AI(string FEN);
	void printDebug(string show);
	class GameTree {
	public:
		GameTree();
		//void search_evaluate(int targetDepth);
	};
private:
	void move(int, int);
	Board board;
};

#endif