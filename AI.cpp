#include "AI.h"

AI::AI(string FEN)
{
	board = Board(FEN);
	debug();
}

void AI::debug()
{
	
}

void AI::printBoard()
{
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