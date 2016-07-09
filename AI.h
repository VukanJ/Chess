#ifndef AI_H
#define AI_H

#include "Board.h"

class AI
{
public:
	AI();
	void move(int, int);
	class GameTree {
	public:
		GameTree();
		//void search_evaluate(int targetDepth);
	};
private:
	Board board;
};

#endif