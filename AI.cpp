#include "AI.h"

AI::AI(string FEN, color computerColor) 
	: aiColor(computerColor)
{
	genChessData data;
	data.gen(); // Generates bitboards needed for move generation
	chessBoard = Board(FEN);
	transposition_hash = ZobristHash(1e7);
	guiActive = false; // Not active for testing purposes.
	//debug();
}

void AI::debug()
{

}

void AI::printBoard()
{
	chessBoard.print();
}

void AI::bindGui(Gui* guiPtr)
{
	gui = guiPtr;
	guiActive = true;
}

string AI::boardToString() const
{
	string board = string(64, '_');
	int c = 0;
	for (auto p : chessBoard.pieces) {
		BITLOOP(pos, p) {
			board[63 - pos] = names[c];
		}
		c++;
	}
	return board;
}

void AI::writeToHistory(const Move& move)
{
	gameHistory.push_back(pair<string, Move>(boardToString(), move));
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

void AI::Play(sf::RenderWindow& window)
{
	Root.reset(new Node());
	chessBoard.updateAllAttacks();
	// Builds gametree and determines best move and plays it
	// Root == current board position with depth = 0

	pair<Move, int> bestMove;
	for (targetDepth = 1; targetDepth < 5; ++targetDepth) {
		bestMove = distributeNegaMax();
		if (targetDepth == 1 && bestMove.second == oo) {
			cout << "*===========*\n| You Lose! |\n*===========*" << endl;
			break;
		}
		gui->visualizeScore(bestMove.second);
		gui->render(window);
		window.display();
	}
	gameHistory.push_back(pair<string, Move>(boardToString(), bestMove.first));
	chessBoard.makeMove(bestMove.first, aiColor);
	chessBoard.updateAllAttacks();

	gui->visualizeLastMove(bestMove.first);
}

void AI::sortMoves(nodePtr& node, color side)
{
	sort(node->moveList.begin(), node->moveList.end(), [this, side](const Move& move1, const Move& move2) {
		chessBoard.makeMove(move1, side);
		int val1 = transposition_hash.getValue(chessBoard.hashKey);
		chessBoard.unMakeMove(move1, side);
		chessBoard.makeMove(move2, side);
		int val2 = transposition_hash.getValue(chessBoard.hashKey);
		chessBoard.unMakeMove(move2, side);
		return val1 < val2;
	});
}

pair<Move, int> AI::distributeNegaMax()
{
	typedef pair<Move, int> moveValue;
	// distributeNegaMax() distributes negamax to all children of the Root node
	// and tracks the move value associated with the move
	static vector<moveValue> RootMoveList;
	// If Root value has not been calculated for current position:
	chessBoard.updateAllAttacks();
	if (!(Root->nodeFlags & Node::Flags::explored)) {
		// Generate moves for computer player
		// The computer is the maximizing player
		chessBoard.generateMoveList(Root->moveList, aiColor);

		// Clean initial move list at Root
		for (auto move = Root->moveList.begin(); move != Root->moveList.end();) {
			chessBoard.makeMove(*move, aiColor);
			chessBoard.updateAllAttacks();
			if (chessBoard.isKingInCheck(aiColor)) {
				// Black king is in check, invalid move
				chessBoard.unMakeMove(*move, aiColor);
				move = Root->moveList.erase(move);
				continue;
			}
			chessBoard.unMakeMove(*move, aiColor);
			move++;
		}
		if (Root->moveList.empty()) {
			// End of game. Human wins :/
			cout << "*===========*\n| You Win! |\n*===========*" << endl;
			return moveValue(Move(), 0);
		}
		// combine moves with their values
		RootMoveList = vector<moveValue>(Root->moveList.size(), moveValue(Move(), oo));
		for (int i = 0; i < RootMoveList.size(); ++i) {
			Root->nodeList.push_back(nodePtr(new Node()));
		}

		for (int i = 0; i < Root->moveList.size(); ++i)
			RootMoveList[i].first = Root->moveList[i];
	}

	// Distribute moves to NegaMax if needed
	int moveIter = 0;
	for (auto& move_val : RootMoveList) {
		chessBoard.makeMove(move_val.first, aiColor);
		chessBoard.updateAllAttacks();
		auto entry = transposition_hash.getEntry(chessBoard.hashKey);
		if (entry.search_depth >= targetDepth) {
			// Move has been evaluated well enough before 
			move_val.second = entry.value;
		}
		else {
			move_val.second = -NegaMax_Search(Root->nodeList[moveIter++], -oo, oo, targetDepth, aiColor == black ? white : black);
		}
		chessBoard.unMakeMove(move_val.first, aiColor);
	}

	auto bestMove = max_element(RootMoveList.begin(), RootMoveList.end(),
		[](const moveValue move1, const moveValue move2) {
		return move1.second < move2.second;
	});

	Root->nodeFlags |= Node::Flags::explored;
	//printf("Best move for computer is %s with value %d\n", moveString(bestMove->first).c_str(), bestMove->second);
	//printf("From: %d, To: %d, type: %d, mpiece: %c tpiece: %c\n", bestMove->first.from,
	//															  bestMove->first.to,
	//															  bestMove->first.flags,
	//															  names[move_piece(bestMove->first.Pieces)],
	//															  names[target_piece(bestMove->first.Pieces)]);
	return *bestMove;
}

int AI::NegaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side)
{
	auto hashEntry = transposition_hash.getEntry(chessBoard.hashKey);
    int oldAlpha = alpha;

	if (hashEntry.search_depth >= targetDepth - depth) { 
		// If stored move value is of higher quality use 
		// data from previous search
		if (hashEntry.flags & EXACT_VALUE) {
			return hashEntry.value;
		}
		else if (hashEntry.flags & LOWER_BOUND) {
			alpha = max(alpha, hashEntry.value);
		}
		else if (hashEntry.flags & UPPER_BOUND) {
			beta = min(beta, hashEntry.value);
		}
		if (alpha >= beta) { // Alpha Beta Cutoff
			node->nodeFlags |= Node::Flags::cutNode;
			return hashEntry.value;
		}
	}

	if (depth == 0 || node->nodeFlags & Node::Flags::terminal)
		return chessBoard.evaluate(side);

	int bestValue = -oo, boardValue = -oo;

	// Calculate all possible moves if necessary:
	if (node->nodeFlags & Node::Flags::explored)
		// Node already exists in memory and has children
		sortMoves(node, side);
	else 
		chessBoard.generateMoveList(node->moveList, side);

	for (auto move = node->moveList.begin(); move != node->moveList.end();) {
		chessBoard.makeMove(*move, side);
		chessBoard.updateAllAttacks();
		if (chessBoard.isKingInCheck(side)) {
			// Invalid move: King left in check
			chessBoard.unMakeMove(*move, side);
			move = node->moveList.erase(move);
			continue;
		}
		// Move is legal, append:
		node->nodeList.push_back(nodePtr(new Node()));

		boardValue = -NegaMax_Search(node->nodeList.back(), -beta, -alpha, depth - 1, side == black ? white : black);
		bestValue = max(bestValue, boardValue);
		alpha = max(alpha, boardValue);
		if (alpha >= beta) {
			// Stop search, bad strategy encountered
			node->nodeFlags |= Node::Flags::cutNode;
			chessBoard.unMakeMove(*move, side);
			break;
		}

		chessBoard.unMakeMove(*move, side);
		move++;
	}
	node->nodeFlags |= Node::Flags::explored; // Moves can be sorted in next iteration (iterative deepening)

	if (node->moveList.empty()) {
		// This player has no possible legal moves
		node->nodeFlags |= Node::Flags::terminal;
		if(chessBoard.isKingInCheck(side))
			 return -oo;          // Checkmate
		else return -oo + 100000; // Stalemate, slightly better than being checkmated
	}

	// Initialize hash entry
	hashEntry.value = bestValue;
	if (bestValue < oldAlpha) {
		hashEntry.flags = UPPER_BOUND;
	}
	else if (bestValue >= beta) {
		hashEntry.flags = LOWER_BOUND;
	}
	else {
		hashEntry.flags = EXACT_VALUE;
	}
	hashEntry.search_depth = targetDepth - depth;
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

AI::Node::Node() : nodeFlags(0x0) {}