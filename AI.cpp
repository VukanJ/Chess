#include "AI.h"

AI::AI(string FEN)
{
	chessBoard = Board(FEN);
	debug();
	sideToMove = black; // To be implemented
}

void AI::debug()
{
	
}

void AI::printBoard()
{
	chessBoard.print();
}

void AI::printDebug(string showPieces)
{
	chessBoard.print();
	for (auto p : showPieces){
		auto drawPiece = getPieceIndex(p);
		///printBitboard(chessBoard.pieces[drawPiece]);
		///printBitboard(chessBoard.attacks[drawPiece]);
		if (chessBoard.pieces[drawPiece])
			printBitboardFigAttack(chessBoard.pieces[drawPiece], chessBoard.attacks[drawPiece], p);
	}
}

void AI::negaMax_Search(Node* node, int depth)
{
	// Calculate move list
	// Sort moves
	// Evaluate board(s)
	// Use negamax
	chessBoard.generateMoveList(node->moves, black);
	for (auto mov = node->moves.begin(); mov != node->moves.end();){
		chessBoard.makeMove(*mov);
		if(!chessBoard.hash.hasEntry(chessBoard.hashKey))
			node->boardScore = chessBoard.evaluate();
		else node->boardScore = chessBoard.hash.getEntry(chessBoard.hashKey, node->boardScore,);

		mov++;
	}
	// Unfinished
}