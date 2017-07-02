#include "Testing.h"

SearchTest::SearchTest() 
	: transpositionHash(ZobristHash(1e7)),
	targetDepth(1), evalcnt(0), negaMaxCnt(0), 
	storedBoards(0), hashAccess(0), moveCnt(0), finished(0), ordering(0.9) 
{
	PVtable = vector<Move>(5050, Move());
}

void SearchTest::test()
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation

	// string FEN = "4B3/5p2/qpN4K/r2R1Q2/1Pk5/4P3/1R6/1Nnr3b w - - 1 0"; // Mate in two
	//string FEN = "r4r1k/1bpq1p1n/p1np4/1p1Bb1BQ/P7/6R1/1P3PPP/1N2R1K1 w - - 1 0"; // Mate in 8
	//string FEN = "2r2k1r/pB3pp1/4p3/Q1p1P3/3P4/2P1q1p1/PP3RP1/5RK1 w - - 1 0"; // ate in 3
	//string FEN = "8/B7/88/4n3/5kpp/8/7K b - - 1 0";
	string FEN = "5n2/K7/8/2B5/kP6/N1r5/R1P5/1Q6 w - - 1 0";

	board.setupBoard(FEN);
	board.updateAllAttacks();
	board.print();

	auto bestMove = getBestMove(white);
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
		board.makeMove<PROPER>(move, side);
		if (board.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<PROPER>(move, side);
			continue;
		}
		int value = transpositionHash.getValue(board.hashKey);
		if (bestvalue < value) {
			bestvalue = value;
			bestMove = transpositionHash.getEntry(board.hashKey).bestMove;
		}
		board.unMakeMove<PROPER>(move, side);
	}
	return transpositionHash.getEntry(board.hashKey).bestMove;
}

Move SearchTest::getBestMove(color forPlayer)
{
	Move bestMove;
	Timer timer;
	auto oldHash = board.hashKey;

	for (targetDepth = 1; targetDepth < 20; targetDepth++) {
		timer.start();
		NegaMax(-oo, oo, targetDepth, forPlayer, forPlayer);
		timer.stop();

		cout << "Depth: " << targetDepth << endl;
		cout << "Time: " << timer.getTime()*1e-6 << endl;

		cout << "PV: " << '(' << transpositionHash.getValue(board.hashKey) << ") ";
		extractPrincipalVariation(board.hashKey, targetDepth, forPlayer);
		cout << ".\n";
	}
	assert(oldHash == board.hashKey);
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

MoveList::iterator SearchTest::nextMove(MoveList& moveList, MoveList::iterator& nextMove, color side)
{
	// Sorting scheme:
	// 1. Alpha Beta Value
	// 2. MVV-LVA
	// 3. Killer-Moves
	// Sort moves according to previously determined alpha-beta values
	int maxValue = -oo;
	MoveList::iterator maxMove = nextMove; 
	for (auto move = nextMove; move < moveList.end(); move++) {
		board.makeMove<HASH_ONLY>(*move, side);
		int value = transpositionHash.getEntry(board.hashKey).value;
		if (value > maxValue) {
			maxValue = value;
			maxMove  = move;
		}
		board.unMakeMove<HASH_ONLY>(*move, side);
	}
	swap(nextMove, maxMove);
	return nextMove;
	// MVV-LVA
	//for (auto iter = moveList.rend(); iter > MoveList::reverse_iterator(nextMove); iter++) {
	//	if (iter->mtype() == CAPTURE || iter->mtype() == C_PROMOTION) {
	//
	//	}
	//}
}

int SearchTest::NegaMax(int alpha, int beta, int depth, color aiColor, color side)
{
	try {
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
			return board.evaluate(side);
		}

		int bestValue = -oo;
		MoveList movelist;
		Move bestMove;

		board.generateMoveList(movelist, side, true);
		bool checkOnThisDepth = board.wasInCheck;
		U64 pinnedOnThisDepth = board.pinned;

		// Move ordering here
		for (auto move = movelist.begin();
			move != movelist.end(); move++) {
			if (targetDepth == 1) cout << *move << endl;
			board.makeMove<PROPER>(*move, side);
			board.updateAllAttacks();
			if (board.isKingLeftInCheck(side, *move, checkOnThisDepth, pinnedOnThisDepth)) {
				board.unMakeMove<PROPER>(*move, side);
				continue;
			}
			int score;

			score = -NegaMax(-beta, -alpha, depth - 1, aiColor, !side);

			board.unMakeMove<PROPER>(*move, side);

			if (score >= bestValue) {
				bestValue = score;
			}
			if (score >= alpha) {
				if (move == movelist.end()) {
					cout << "OK";
				}
				bestMove = *move;
				// Found better move
				alpha = score;
			}
			// Update move value and get next most promising move
			if (alpha >= beta) {
				break; // Alpha beta cutoff
			}
		}
		if (movelist.empty()) {
			// This player has no possible legal moves
			board.updateAllAttacks();
			if (board.isKingInCheck(side)) {
				bestValue = -oo; // Checkmate
			}
			else {
				bestValue = -oo + 100000; // Stalemate, slightly better than being checkmated
			}
		}

		// Store board in transposition table
		storedBoards++;
		entry.bestMove = bestMove;
		entry.value = bestValue;
		if (bestValue <= oldAlpha)
			entry.flags = UPPER_BOUND;
		else if (bestValue >= beta)
			entry.flags = LOWER_BOUND;
		else {
			entry.flags = EXACT_VALUE;
		}
		entry.search_depth = depth;

		return bestValue;
	}
	catch (...) {

	}
}

int SearchTest::QuiescenceSearch(int alpha, int beta, int depth, color aiColor, color side)
{
	return 0;
}

void SearchTest::extractPrincipalVariation(const U64& key, int maxPrintDepth, color side)
{
	if (maxPrintDepth == 0) return; // If loops (transpositions) occur abort 
	const auto& entry = transpositionHash.getEntry(key);
	if (entry.search_depth == -1 || entry.bestMove.from == 255) return;
	board.makeMove<PROPER>(entry.bestMove, side);
	cout << entry.bestMove << "  ";
	extractPrincipalVariation(board.hashKey, maxPrintDepth - 1, !side);
	board.unMakeMove<PROPER>(entry.bestMove, side);
}