#include "Testing.h"

SearchTest::SearchTest()
	: transpositionHash(ZobristHash(1e7)),
	targetDepth(1), evalcnt(0), negaMaxCnt(0),
	storedBoards(0), hashAccess(0), moveCnt(0), finished(0), ordering(0.9){}

// TODO: Quiescence search
// TODO: Move sorting

void SearchTest::test()
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation

	//string FEN = "4B3/5p2/qpN4K/r2R1Q2/1Pk5/4P3/1R6/1Nnr3b w - - 1 0"; // Mate in two
	//string FEN = "r4r1k/1bpq1p1n/p1np4/1p1Bb1BQ/P7/6R1/1P3PPP/1N2R1K1 w - - 1 0"; // Mate in 8

	string FEN = "*";
	//string FEN = "2r2k1r/pB3pp1/4p3/Q1p1P3/3P4/2P1q1p1/PP3RP1/5RK1 w - - 1 0"; // Mate in 3

	//string FEN = "8/pk1B4/p7/2K1p3/8/8/4Q3/8 w - - 1 0"; // Bh3 a5 Qa6 Kxa6 Bc8#
	//string FEN = "k7/pp1K4/N7/8/8/8/8/7B w - - 1 0";

	//string FEN = "7R/8/8/k1K5/8/8/8/8 w - - 1 0"; // Mate in 0.5

	// Mate in 4, Albert Becker vs. Eduard Glass 1928
	// Time: 37s, (+primitive TT) -> 24s, (+TT lookup) -> 9s
	//string FEN = "rk6/N4ppp/Qp2q3/3p4/8/8/5PPP/2R3K1 w - - 1 0"; // Rc1c8  qx{R}e6c8  Qx{p}a6b6  qc8b7  Na7c6  kb8c8  Qb6d8

	// https://www.sparkchess.com/chess-puzzles.html
	//string FEN = "4r1k1/pQ3pp1/7p/4q3/4r3/P7/1P2nPPP/2BR1R1K b - - 1 0";
	//string FEN = "4Rnk1/pr3ppp/1p3q2/5NQ1/2p5/8/P4PPP/6K1 w - - 1 0"; // Nf5h6  qx{N}f6h6  Rx{n}e8f8  kx{R}g8f8  Qg5d8
	//string FEN = "4k2r/1R3R2/p3p1pp/4b3/1BnNr3/8/P1P5/5K2 w - - 1 0";
	//string FEN = "4r3/pbpn2n1/1p1prp1k/8/2PP2PB/P5N1/2B2R1P/R5K1 w - - 1 0"; // Mate in 2
	// string FEN = "7r/p3ppk1/3p4/2p1P1Kp/2Pb4/3P1QPq/PP5P/R6R b - - 0 1";
	
	board.setupBoard(FEN);
	board.print();

	aiColor = black;
	auto bestMove = getBestMove(aiColor);
}

Move SearchTest::getBestMove(color forPlayer)
{
	Move bestMove;
	Timer timer, totalTimer;

	totalTimer.start();
	for (targetDepth = 1; targetDepth < 9; targetDepth++) {
		timer.start();
		auto oldHash = board.hashKey;
		NegaMax(-oo, oo, targetDepth, 0, forPlayer);
		assert(oldHash == board.hashKey);
		timer.stop();

		cout << "D:{" << targetDepth << '}';
		cout << " T:{" << (int)(timer.getTime()*1e-6) << "} ";
		int val = transpositionHash.getValue(board.hashKey);
		cout << "PV: " << '(' << (abs(val) == oo ? (val > 0 ? "oo" : "-oo") : to_string(val)) << ") ";
		extractPrincipalVariation(board.hashKey, targetDepth, forPlayer);
		cout << ".\n";
		cout << negaMaxCnt << " Nodes\n";
		negaMaxCnt = 0;
	}
	board.print();
	totalTimer.stop();
	cout << "Total time = " << totalTimer.getTime()*1e-6 << endl;
	return bestMove;
}

void SearchTest::nextMove(MoveList& mlist, const MoveList::iterator& nextMove, color side)
{
	// Sorting scheme:
	// 1. Alpha Beta Value
	// 2. MVV-LVA
	// 3. Killer-Moves
	// Sort moves according to previously determined alpha-beta values

	//return;

	/*
	int maxValue = -oo;
	MoveList::iterator maxMove = nextMove;
	for (auto move = nextMove; move != mlist.end(); move++) {
		board.makeMove<HASH>(*move, side);
		const auto& entry =transpositionHash.getEntry(board.hashKey);
	
		if (entry.search_depth != -1 && (side == black ? -1 : 1) *entry.value > maxValue) {
			maxValue = entry.value;
			maxMove = move;
		}
		board.unMakeMove<HASH>(*move, side);
	}

	iter_swap(nextMove, maxMove);
	*/


	// MVV-LVA
	//for (auto iter = moveList.rend(); iter > MoveList::reverse_iterator(nextMove); iter++) {
	//	if (iter->mtype() == CAPTURE || iter->mtype() == C_PROMOTION) {
	//
	//	}
	//}
}

int SearchTest::NegaMax(int alpha, int beta, int depth, int ply, color side)
{
	negaMaxCnt++;
	int oldAlpha = alpha;
	auto &entry = transpositionHash.getEntry(board.hashKey);

	// Check if move was already evaluated
	if (entry.search_depth >= depth || entry.terminal == 1) {
		if (entry.flags & EXACT_VALUE) {
			return entry.value;
		}
		else if (entry.flags & LOWER_BOUND) {
			alpha = max(alpha, entry.value);
		}
		else if (entry.flags & UPPER_BOUND) {
			beta = min(beta, entry.value);
		}
		if (alpha >= beta) {
			return entry.value;
		}
	}

	if (depth == 0) {
		//return board.evaluate(side);
		return QuiescenceSearch(alpha, beta, 0, side);
	}

	MoveList movelist;
	board.updateAllAttacks();
	board.generateMoveList<ALL>(movelist, side);

	bool checkedOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	int legalMoves = 0, score = 0;
	Move bestMove;

	for (const auto& move : movelist) {
		board.makeMove<FULL>(move, side);
		//board.updateAllAttacks();

		if (board.isKingLeftInCheck(side, move, checkedOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<FULL>(move, side);
			//board.updateAllAttacks();
			continue;
		}
		legalMoves++;
		score = -NegaMax(-beta, -alpha, depth - 1, ply + 1, !side);

		board.unMakeMove<FULL>(move, side);
		//board.updateAllAttacks(); // Maybe not needed
		if (score > alpha) {
			if (score >= beta) {
				return beta; // Beta Cutoff
			}
			alpha = score;
			bestMove = move;
		}
	}
	if (legalMoves == 0) {
		// No legal moves found. 
		board.updateAllAttacks();
		if ((side == white && board.pieces[wk] & board.blackAtt)
			|| (side == black && board.pieces[bk] & board.whiteAtt)) {
			// Checkmate, end of game path
			entry.search_depth = depth;
			entry.terminal = true;
			score = -oo + ply;
			return score;
		}
		else {
			// Stalemate, end of game path
			entry.search_depth = depth;
			entry.terminal = true;
			score = 0;
			return 0;
		}
	}
	if (alpha > oldAlpha) {
		// Better move found
		pvTable.addPVMove(board.hashKey, bestMove);
		entry.value = alpha;
		entry.search_depth = depth;
	}
	if (alpha < oldAlpha) {
		entry.flags = UPPER_BOUND;
	}
	else if (alpha >= beta)
		entry.flags = LOWER_BOUND;
	else entry.flags = EXACT_VALUE;

	return alpha;
}

int SearchTest::QuiescenceSearch(int alpha, int beta, int ply, color side)
{
	int standingPat = board.evaluate(side);
	//if (ply > 2) {
	//	return standingPat;
	//}
	if (standingPat >= beta) 
		return beta;
	if (alpha < standingPat)
		alpha = standingPat;

	MoveList mlist;
	board.updateAllAttacks();
	board.generateMoveList<CAPTURES_ONLY>(mlist, side);

	// MVA-LLV scheme
	negaMaxCnt++;
	stable_sort(mlist.begin(), mlist.end(), [](const Move& m1, const Move& m2) {
		return captureScore[m1.movePiece() % 6][m1.targetPiece() % 6] 
			 > captureScore[m2.movePiece() % 6][m2.targetPiece() % 6];
	});

	bool checkOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	for (const auto& capture : mlist) {
		board.makeMove<FULL>(capture, side);
		if (board.isKingLeftInCheck(side, capture, checkOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<FULL>(capture, side);
			continue;
		}

		int score = -QuiescenceSearch(-beta, -alpha, ply + 1, !side);
		board.unMakeMove<FULL>(capture, side);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

void SearchTest::invertChessboard()
{
	/*
	for (int i = 0; i < 6; ++i)
		swap(board.pieces[i], board.pieces[i + 6]);
	for (auto& piece : board.pieces)
		piece = _byteswap_uint64(piece);
	board.updateAllAttacks();
	transpositionHash.clear();
	board.print();
	*/
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
		board.makeMove<POS>(move, side);
		if (board.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth))
			invalid++;
		board.unMakeMove<POS>(move, side);
	}
	return invalid == ml.size() ? true : false;
}

void SearchTest::extractPrincipalVariation(const U64& key, int maxPrintDepth, color side)
{
	const auto& entry = pvTable[key];
	if (maxPrintDepth == 0 || entry.bestmove.invalid()) return;
	board.makeMove<FULL>(entry.bestmove, side);
	cout << moveString(entry.bestmove) << "  ";
	extractPrincipalVariation(board.hashKey, maxPrintDepth - 1, !side);
	board.unMakeMove<FULL>(entry.bestmove, side);
}
