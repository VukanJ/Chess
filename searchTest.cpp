#include "Testing.h"

SearchTest::SearchTest() 
	: transpositionHash(ZobristHash(1e7)),
	targetDepth(1), evalcnt(0), negaMaxCnt(0), 
	storedBoards(0), hashAccess(0), moveCnt(0), finished(0), ordering(0.9){}

// TODO: Zugzwang detection
// TODO: Quiescence search 
// TODO: Move sorting

void SearchTest::test()
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation

	//string FEN = "4B3/5p2/qpN4K/r2R1Q2/1Pk5/4P3/1R6/1Nnr3b w - - 1 0"; // Mate in two
	//string FEN = "r4r1k/1bpq1p1n/p1np4/1p1Bb1BQ/P7/6R1/1P3PPP/1N2R1K1 w - - 1 0"; // Mate in 8
	
	//string FEN = "*";
	string FEN = "2r2k1r/pB3pp1/4p3/Q1p1P3/3P4/2P1q1p1/PP3RP1/5RK1 w - - 1 0"; // Mate in 3

	//string FEN = "8/pk1B4/p7/2K1p3/8/8/4Q3/8 w - - 1 0"; // Bh3 a5 Qa6 Kxa6 Bc8#
	//string FEN = "k7/pp1K4/N7/8/8/8/8/7B w - - 1 0";

	//string FEN = "7R/8/8/k1K5/8/8/8/8 w - - 1 0"; // Mate in 0.5

	//string FEN = "5n2/K7/8/2B5/kP6/N1r5/R1P5/1Q6 w - - 1 0";

	board.setupBoard(FEN);
	board.print();

	auto bestMove = getBestMove(black);
}

Move SearchTest::getMaxRootMove(color side)
{
	MoveList movelist;
	board.updateAllAttacks();
	board.generateMoveList(movelist, side, true);
	Move bestMove;
	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	int bestvalue = -oo;
	for (auto& move : movelist)
	{
		board.makeMove<FULL>(move, side);
		if (board.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<FULL>(move, side);
			continue;
		}
		int value = transpositionHash.getValue(board.hashKey);
		if (bestvalue < value) {
			bestvalue = value;
			bestMove = transpositionHash.getEntry(board.hashKey).bestMove;
		}
		board.unMakeMove<FULL>(move, side);
	}
	return transpositionHash.getEntry(board.hashKey).bestMove;
}

Move SearchTest::getBestMove(color forPlayer)
{
	Move bestMove;
	Timer timer, totalTimer;

	totalTimer.start();
	for (targetDepth = 1; targetDepth < 8; targetDepth++) {
		timer.start();
		auto oldHash = board.hashKey;
		NegaMax(-oo, oo, targetDepth, forPlayer, forPlayer);
		assert(oldHash == board.hashKey);
		timer.stop();
		
		cout << "D:{" << targetDepth << '}';
		cout << " T:{" << (int)(timer.getTime()*1e-6) << "} ";
		int val = transpositionHash.getValue(board.hashKey);
		cout << "PV: " << '(' << (abs(val) == oo ? (val > 0 ? "oo" : "-oo") : to_string(val)) << ") ";
		extractPrincipalVariation(board.hashKey, targetDepth, forPlayer);
		cout << ".\n";
	}
	totalTimer.stop();
	cout << "Total time = " << totalTimer.getTime()*1e-6 << endl;
	return bestMove;
}

//Move SearchTest::distributeNegaMax(color forPlayer)
//{
//	typedef pair<Move, int> MoveValue;
//	static vector<MoveValue> RootMoveList;
//	MoveList moveList;
//	board.updateAllAttacks();
//
//	bool checkOnThisDepth = board.wasInCheck;
//	U64 pinnedOnThisDepth = board.pinned;
//
//	if (targetDepth == 1) {
//		board.generateMoveList(moveList, forPlayer, true);
//		
//
//		checkOnThisDepth = board.wasInCheck;
//		pinnedOnThisDepth = board.pinned;
//
//		for (auto move = moveList.begin(); move != moveList.end(); ) {
//			board.makeMove<PROPER>(*move, forPlayer);
//			moveCnt++;
//			auto& entry = transpositionHash.getEntry(board.hashKey);
//			if (board.isKingLeftInCheck(forPlayer, *move, checkOnThisDepth, pinnedOnThisDepth)) {
//				board.unMakeMove<PROPER>(*move, forPlayer);
//				move = moveList.erase(move);
//				continue;
//			}
//			board.unMakeMove<PROPER>(*move, forPlayer);
//			//entry.search_depth = 0;
//			//entry.value = board.evaluate(forPlayer);
//			//entry.bestMove = *move;
//			//entry.flags = EXACT_VALUE;
//			move++;
//		}
//		// Initialize
//		RootMoveList.resize(moveList.size());
//		for (int i = 0; i < moveList.size(); ++i)
//			RootMoveList[i] = pair<Move, int>(moveList[i], -oo);
//	}
//
//	for (auto& move_value : RootMoveList) {
//
//		board.makeMove<PROPER>(move_value.first, forPlayer);
//		auto& entry = transpositionHash.getEntry(board.hashKey);
//		if (entry.search_depth >= targetDepth) {
//			move_value.second = entry.value;
//		}
//		else {
//			move_value.second = -NegaMax(-oo, oo, targetDepth - 1, forPlayer, forPlayer == white ? black : white);
//		}
//		board.unMakeMove<PROPER>(move_value.first, forPlayer);
//	}
//
//	auto bestMove = max_element(RootMoveList.begin(), RootMoveList.end(),
//		[](const MoveValue move1, const MoveValue move2) {
//		return move1.second < move2.second;
//	});
//
//	// Update Root Node
//	auto& RootEntry = transpositionHash.getEntry(board.hashKey);
//	RootEntry.search_depth = targetDepth;
//	RootEntry.bestMove = bestMove->first;
//	RootEntry.value = bestMove->second;
//
//	return bestMove->first;
//}

void SearchTest::nextMove(MoveList& mlist, const MoveList::iterator& nextMove, color side)
{
	// Sorting scheme:
	// 1. Alpha Beta Value
	// 2. MVV-LVA
	// 3. Killer-Moves
	// Sort moves according to previously determined alpha-beta values
	
	if (side == white) {
		int maxValue = -oo;
		MoveList::iterator maxMove = nextMove;
		for (auto move = nextMove; move != mlist.end(); move++) {
			board.makeMove<HASH>(*move, side);
			auto entry = transpositionHash.getEntry(board.hashKey);

			if (entry.value > maxValue) {
				maxValue = entry.value;
				maxMove = move;
			}
			board.unMakeMove<HASH>(*move, side);
		}

		iter_swap(nextMove, maxMove);
	}
	else {
		int minValue = oo;
		MoveList::iterator minMove = nextMove;
		for (auto move = nextMove; move != mlist.end(); move++) {
			board.makeMove<HASH>(*move, side);
			auto entry = transpositionHash.getEntry(board.hashKey);

			if (entry.value < minValue) {
				minValue = entry.value;
				minMove = move;
			}
			board.unMakeMove<HASH>(*move, side);
		}

		iter_swap(nextMove, minMove);
	}

	// MVV-LVA
	//for (auto iter = moveList.rend(); iter > MoveList::reverse_iterator(nextMove); iter++) {
	//	if (iter->mtype() == CAPTURE || iter->mtype() == C_PROMOTION) {
	//
	//	}
	//}
}

int SearchTest::NegaMax(int alpha, int beta, int depth, color aiColor, color side)
{
	int oldAlpha = alpha;
	auto &entry = transpositionHash.getEntry(board.hashKey);

	if (entry.search_depth >= depth) {
		if (entry.flags & EXACT_VALUE) {
			return entry.value;
		}
		else if (entry.flags & LOWER_BOUND)
			alpha = max(alpha, entry.value);
		else if (entry.flags & UPPER_BOUND)
			beta = min(beta, entry.value);
		if (alpha >= beta)
			return entry.value;
	}

	if (depth == 0) {
		return board.evaluate(side) - targetDepth + depth;
	}

	int bestValue = -oo;
	MoveList movelist;
	board.updateAllAttacks(); // TODO: Possibly relocate
	board.generateMoveList(movelist, side, true);
	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;
	Move bestMove;

	if (!movelist.empty()) {
		//for (auto move = movelist.begin(); move != movelist.end(); move++) {
		MoveList::iterator move = movelist.begin();
		for (auto i = 0; i < movelist.size(); ++i){
			nextMove(movelist, move, side);
			board.makeMove<FULL>(*move, side);

			board.updateAllAttacks();
			if (board.isKingLeftInCheck(side, *move, checkOnThisDepth, pinnedOnThisDepth)) {
				board.unMakeMove<FULL>(*move, side);
				move = next(move);
				continue;
			}

			int score;
			if ((side == white && board.pieces[bk] & board.whiteAtt && isCheckmate<black>())
				|| (side == black && board.pieces[wk] & board.blackAtt && isCheckmate<white>())) {
				entry.search_depth = depth;
				score = oo - targetDepth + depth;
			}
			else {
				score = -NegaMax(-beta, -alpha, depth - 1, aiColor, !side);
			}
			board.unMakeMove<FULL>(*move, side);
			if (score >= bestValue) {
				bestValue = score;
				bestMove = *move;
			}
			if (score >= alpha) {
				// Found better move
				alpha = score;
			}
			if (alpha >= beta) {
				break; // Alpha beta cutoff
			}
			move = next(move);
		}
		if (bestMove.invalid()) {
			// Bestmove wasnt set => all moves were skipped 
			// Stalemate, end of game path
			entry.search_depth = depth;
			entry.flags = EXACT_VALUE;
			entry.value = 0;
			return 0;
		}
	}
	if (movelist.empty()) {
		// This player has no possible legal moves
		board.updateAllAttacks();
		if (board.isKingInCheck(side)) {
			entry.value = -oo + targetDepth - depth;       // Checkmate -> end of game path
			entry.search_depth = depth; // This evaluation always prevails
			entry.flags = EXACT_VALUE;
			return -oo;
		}
		else {
			entry.value = 0;         // Stalemate -> end of game path
			entry.search_depth = depth; // This evaluation always prevails
			entry.flags = EXACT_VALUE;
			return 0;
		}
	}

	// Store board in transposition table
	entry.bestMove = bestMove;
	entry.value = bestValue;
	entry.search_depth = depth;

	if (bestValue <= oldAlpha)
		entry.flags = UPPER_BOUND;
	else if (bestValue >= beta)
		entry.flags = LOWER_BOUND;
	else {
		entry.flags = EXACT_VALUE;
	}

	return bestValue;
}

template<color side> bool SearchTest::isCheckmate()
{
	// TODO: makemove<POS> does not produce same results in this function as makemove<FULL>
	MoveList ml;
	board.generateMoveList(ml, side, true);
	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	auto invalid = 0;

	for (auto& move : ml) {
		board.makeMove<FULL>(move, side);
		if (board.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth))
			invalid++;
		board.unMakeMove<FULL>(move, side);
	}
	return invalid == ml.size() ? true : false;
}

int SearchTest::QuiescenceSearch(int alpha, int beta, int depth, color aiColor, color side)
{
	return 0;
}

void SearchTest::extractPrincipalVariation(const U64& key, int maxPrintDepth, color side)
{
	const auto& entry = transpositionHash.getEntry(key);
	if (entry.search_depth == -1 || maxPrintDepth == 0 || entry.bestMove.invalid()) return;
	board.makeMove<FULL>(entry.bestMove, side);
	cout << moveString(entry.bestMove) << "  ";
	extractPrincipalVariation(board.hashKey, maxPrintDepth - 1, !side);
	board.unMakeMove<FULL>(entry.bestMove, side);
}