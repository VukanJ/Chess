#include "Testing.h"

SearchTest::SearchTest() 
	: transpositionHash(ZobristHash(1e7)),
	targetDepth(1), evalcnt(0), negaMaxCnt(0), 
	storedBoards(0), hashAccess(0), moveCnt(0) {}

Move SearchTest::getBestMove(color forPlayer)
{
	Move bestMove;
	Timer timer;
	static int count = 1;
	cout << "Board " << count++ << endl;
	board.print();
	auto oldHash = board.hashKey;
	for (targetDepth = 1; targetDepth < 10; targetDepth++) {
		timer.start();
		bestMove = distributeNegaMax(forPlayer);
		timer.stop();
		cout << "Time: " << timer.getTime()*1e-6 << endl;
		cout << string(80, '~') << endl;
		cout << "Depth " << targetDepth << " best move = " << shortNotation(bestMove) << endl;
		cout << "Search Info: \n";
		printf("\t%d\tEvaluations"
			"\n\t%d\tNegaMax Calls"
			"\n\t%d\tHashed boards"
			"\n\t%d\tHash Accesses"
			"\n\t%d\tPlayed Moves\n",
			evalcnt, negaMaxCnt, storedBoards, hashAccess, moveCnt);
		evalcnt = negaMaxCnt = hashAccess = moveCnt = 0;
	}
	assert(oldHash == board.hashKey);
	return bestMove;
}

Move SearchTest::distributeNegaMax(color forPlayer)
{
	typedef pair<Move, int> moveValue;
	static vector<moveValue> RootMoveList;
	MoveList moveList;
	board.updateAllAttacks();

	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	if (targetDepth == 1) {
		board.generateMoveList(moveList, forPlayer, true);

		checkOnThisDepth = board.wasInCheck;
		pinnedOnThisDepth = board.pinned;

		for (auto move = moveList.begin(); move != moveList.end(); ) {
			board.makeMove<PROPER>(*move, forPlayer);
			moveCnt++;
			if (board.isKingLeftInCheck(forPlayer, *move, checkOnThisDepth, pinnedOnThisDepth)) {
				board.unMakeMove<PROPER>(*move, forPlayer);
				move = moveList.erase(move);
				continue;
			}
			board.unMakeMove<PROPER>(*move, forPlayer);
			move++;
		}
		// Initialize
		RootMoveList.resize(moveList.size());
		for (int i = 0; i < moveList.size(); ++i)
			RootMoveList[i] = pair<Move, int>(moveList[i], -oo);
	}

	for (auto& move_value : RootMoveList) {

		board.makeMove<PROPER>(move_value.first, forPlayer);
		auto entry = transpositionHash.getEntry(board.hashKey);
		if (entry.search_depth >= targetDepth) {
			move_value.second = entry.value;
		}
		else {
			move_value.second = -NegaMax(-oo, oo, targetDepth - 1, forPlayer, forPlayer == white ? black : white);
		}
		board.unMakeMove<PROPER>(move_value.first, forPlayer);
	}

	auto bestMove = max_element(RootMoveList.begin(), RootMoveList.end(),
		[](const moveValue move1, const moveValue move2) {
		return move1.second < move2.second;
	});

	return bestMove->first;
}

int SearchTest::NegaMax(int alpha, int beta, int depth, color aiColor, color side)
{
	int oldAlpha = alpha;
	negaMaxCnt++;
	hashAccess++;
	auto &entry = transpositionHash.getEntry(board.hashKey);

	if (entry.search_depth >= targetDepth) {
		if (entry.flags & EXACT_VALUE)
			return entry.value;
		else if (entry.flags & LOWER_BOUND)
			alpha = max(alpha, entry.value);
		else if (entry.flags & UPPER_BOUND)
			beta = min(beta, entry.value);
		if (alpha >= beta)
			return entry.value;
	}

	if (depth == 0) {
		evalcnt++;
		hashAccess--;
		return board.evaluate(side);
	}

	int bestValue = -oo;
	MoveList movelist;
	movelist.reserve(32);
	board.updateAllAttacks();
	board.generateMoveList(movelist, side, true);
	//stable_sort(movelist.begin(), movelist.end(), [&, this](const Move& m1, const Move& m2) { // Sort moves
	//	board.makeMove<HASH_ONLY>(m1, side);
	//	int value1 = transpositionHash.getValue(board.hashKey);
	//	board.unMakeMove<HASH_ONLY>(m1, side);
	//	board.makeMove<HASH_ONLY>(m2, side);
	//	int value2 = transpositionHash.getValue(board.hashKey);
	//	board.unMakeMove<HASH_ONLY>(m2, side);
	//	return value1 <= value2;
	//});
	//auto bestMove = max_element(movelist.begin(), movelist.end(), [&, this](const Move& m1, const Move& m2) {
	//	board.makeMove<HASH_ONLY>(m1, side);
	//	int value1 = transpositionHash.getValue(board.hashKey);
	//	board.unMakeMove<HASH_ONLY>(m1, side);
	//	board.makeMove<HASH_ONLY>(m2, side);
	//	int value2 = transpositionHash.getValue(board.hashKey);
	//	board.unMakeMove<HASH_ONLY>(m2, side);
	//	return value1 < value2;
	//});
	//iter_swap(movelist.begin(), bestMove);

	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;
	// Move ordering here
	for (auto move = movelist.begin(); move != movelist.end(); ) {
		//board.print();
		board.makeMove<PROPER>(*move, side);
		moveCnt++;
		if (board.isKingLeftInCheck(side, *move, checkOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<PROPER>(*move, side);
			move = movelist.erase(move);
			//move++;
			continue;
		}
		int currentValue = -NegaMax(-beta, -alpha, depth - 1, aiColor, side == white ? black : white);
		board.unMakeMove<PROPER>(*move, side);
		move++;
		bestValue = max(bestValue, currentValue);
		alpha = max(alpha, currentValue);
		if (alpha >= beta) {
			break; // Alpha beta cutoff
		}
	}

	if (movelist.empty()) {
		// This player has no possible legal moves
		board.updateAllAttacks();
		if (board.isKingInCheck(side))
			return -oo;          // Checkmate
		else return -oo + 100000; // Stalemate, slightly better than being checkmated
	}

	// Store board in transposition table
	storedBoards++;
	entry.value = bestValue;
	if (bestValue < oldAlpha)
		entry.flags = UPPER_BOUND;
	else if (bestValue >= beta)
		entry.flags = LOWER_BOUND;
	else
		entry.flags = EXACT_VALUE;
	entry.search_depth = targetDepth - depth;

	return bestValue;
}
