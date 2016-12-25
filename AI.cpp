#include "AI.h"

AI::AI(string FEN, color computerColor) : aiColor(computerColor)
{
	genChessData data;
	data.gen(); // Generates bitboards needed for move generation
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

void AI::Play()
{
	targetDepth = 1;
	// Builds gametree and determines best move and plays it
	// Root == current board position with depth = 0
	for (;;) { 
		// Loop until time is up or set maximum iteration 
		// depth is reached (depends on game options) 
		negaMax_Search(Root, 0, aiColor);
		targetDepth++;
	}
}

void AI::negaMax_Search(nodePtr& node, int depth, color side)
{
	// TODO: Check if mate
	// Calculate move list
	// Sort moves
	// Evaluate board(s)
	// Use negamax
	// Toggle side to move
	///chessBoard.generateMoveList(node->moves, sideToMove);
	///for (auto move = node->moves.begin(); move != node->moves.end();){
	///	// Do all moves
	///	if (chessBoard.makeMove(*move, side)) {
	///		// Move invalid => discard
	///		chessBoard.unMakeMove(*move, side);
	///		move = node->moves.erase(move);
	///	}
	///	else {
	///		// If BoardPosition already known, use hashed value
	///		if (!chessBoard.hash.hasEntry(chessBoard.hashKey))
	///			node->boardValue = chessBoard.evaluate();
	///		else chessBoard.hash.getEntry(chessBoard.hashKey, node->boardValue);
	///	}
	///}
}

const Board& AI::getBoardRef()
{
	return chessBoard;
}

Board* AI::getBoardPtr()
{
	return &chessBoard;
	//return static_cast<const Board*>(&chessBoard);
}

AI::Node::Node() : ordering(0), boardValue(0), alpha(-oo), beta(oo){
	moves.reserve(10);
	nodes.reserve(10);
}