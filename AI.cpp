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
	targetDepth = 4;
	Root = nullptr;
	Root.reset(new Node());
	// Builds gametree and determines best move and plays it
	// Root == current board position with depth = 0
	///for (;;) { 
	///	// Loop until time is up or set maximum iteration 
	///	// depth is reached (depends on game options) 
	///	/// Call Negamax here
	///	targetDepth++;
	///}
	chessBoard.print();
	auto bHash = chessBoard.hashKey;
	auto bestValue = negaMax_Search(Root, -oo, oo, targetDepth, aiColor == black ? white : black);
	assert(bHash == chessBoard.hashKey);
	if(bestValue == oo) cout << "Best Value = Infinity" << endl;
	else if(bestValue == -oo) cout << "Best Value = -Infinity" << endl;
	else cout << "Best Value = " << bestValue << endl;
	chessBoard.print();
	printBitboard(chessBoard.pieces[bp]);
	printBitboard(chessBoard.pieces[wp]);
}

// TODO: Maybe include "void Board::updateAllAttacks()" in "bool Board::Make/UnmakeMove()"

int AI::negaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side)
{
	// Calculates the minimax boardvalue of a certain position with the negaMax algorithm
	// and stores the move that leads to this position (Principal variation).
	int bestValue = -oo, boardValue; // Initialize worst case scenario

	if (depth == 0) { // Get value of leaf
		if (chessBoard.hash.hasBetterEntry(chessBoard.hashKey, targetDepth - depth)) {
			// Use pre-calculated value if it exists
			chessBoard.hash.getEntry(chessBoard.hashKey, boardValue);
		}
		else {
			// Else make new hash-entry and evaluate board
			boardValue = chessBoard.evaluate(side, targetDepth - depth);
			chessBoard.hash.addEntry(chessBoard.hashKey, boardValue, targetDepth - depth);
		}
		return boardValue;
	}
	// Generate all semi-legal moves and iterate over them
	chessBoard.generateMoveList(node->moveList, side);
	for (auto move = node->moveList.begin(); move != node->moveList.end();) {
		auto saveHash = chessBoard.hashKey;
		chessBoard.makeMove(*move, side);
		// Check if move is legal:
		if (chessBoard.pieces[side == black ? bk : wk] 
			& (side == black ? chessBoard.whiteAtt : chessBoard.blackAtt)) {
			chessBoard.unMakeMove(*move, side);
			if (chessBoard.hashKey != saveHash) cerr << moveString(*move) << endl;
			move = node->moveList.erase(move);
			if (node->moveList.empty()) {
				// Checkmate
				boardValue = oo; 
			}
			continue;
		}
		//if (node->moveList.empty()) {
		//	// Stalemate, prefer mate over stalemate
		//	boardValue = oo - 300000; 
		//}
		node->nodeList.push_back(nodePtr(new Node()));
		boardValue = -negaMax_Search(node->nodeList.back(), -beta, -alpha, depth - 1, side == black ? white : black);
		bestValue = max(bestValue, boardValue);
		alpha = max(alpha, boardValue);
		// Irrelevant subtree found ? 
		if (alpha >= beta) {
			// Skip examination of this subtree
			chessBoard.unMakeMove(*move, side);
			if (chessBoard.hashKey != saveHash) cerr << moveString(*move) << endl;
			break;
		}
		chessBoard.unMakeMove(*move, side);
		if (chessBoard.hashKey != saveHash) cerr << moveString(*move) << endl;
		move++;
	}
	return bestValue;
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

AI::Node::Node(){}