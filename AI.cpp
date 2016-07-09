#include "AI.h"

AI::AI()
{

}

void AI::move(int from, int to)
{
	for (auto& p : board.pieces) {
		if (p & (u64)1 << from && (p & ((u64)1 << to)) == 0) {
			p ^= (u64)1 << from;
			p |= (u64)1 << to;
		}
	}
	board.print();
}