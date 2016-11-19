#include "AI.h"

AI::AI(string FEN)
{
	genChessData data;
	data.gen();
	chessBoard = Board(FEN);
	debug();
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

void AI::negaMax_Search(unique_ptr<AI::Node*> node, int depth)
{
	// Calculate move list
	// Sort moves
	// Evaluate board(s)
	// Use negamax

	///chessBoard.generateMoveList(node->moves, black);
	///for (auto mov = node->moves.begin(); mov != node->moves.end();){
	///	chessBoard.makeMove(*mov);
	///	if(!chessBoard.hash.hasEntry(chessBoard.hashKey))
	///		node->boardScore = chessBoard.evaluate();
	///	else node->boardScore = chessBoard.hash.getEntry(chessBoard.hashKey, node->boardScore);
	///	// calc alpha beta and return if neccessary
	///	// negaMax_Search()
	///	// mov++;
	///}
	// Unfinished
}

const Board& AI::getBoardRef()
{
	return chessBoard;
}
