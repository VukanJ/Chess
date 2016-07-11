#include "AI.h"

AI::AI(string FEN)
{
	board = Board(FEN);
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

void AI::printDebug(string showPieces)
{
	board.print();
	for (auto p : showPieces){
		auto drawPiece = getPieceIndex(p);
		///printBitboard(board.pieces[drawPiece]);
		///printBitboard(board.attacks[drawPiece]);
		printBitboardFigAttack(board.pieces[drawPiece], board.attacks[drawPiece], p);
	}
}