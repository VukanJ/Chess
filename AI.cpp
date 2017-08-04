#include "AI.h"

AI::AI(string FEN, color computerColor)
	: aiColor(computerColor), nodesVisited(0),
	currentAge(0)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation
	board.setupBoard(FEN);
	transpositionHash = ZobristHash(static_cast<size_t>(1e7));
}

AI::AI(string FEN, color computerColor, uint hashSize)
	: aiColor(computerColor)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation
	board.setupBoard(FEN);
	transpositionHash = ZobristHash(hashSize);
}

void AI::printBoard()
{
	board.print();
}

/*
string AI::boardToString() const
{
	string board = string(64, '_');
	int c = 0;
	for (auto p : board.pieces) {
		for_bits(pos, p) {
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
*/

void AI::printDebug(string showPieces)
{
	board.print();
	for (auto p : showPieces){
		auto drawPiece = getPieceIndex(p);
		///printBitboard(chessBoard.pieces[drawPiece]);
		///printBitboard(chessBoard.attacks[drawPiece]);
		if (board.pieces[drawPiece])
			printBitboardFigAttack(board.pieces[drawPiece], board.attacks[drawPiece], p);
	}
}

pair<Move, Move> AI::getBestMove(color forPlayer, int maxDepth, bool uciInfo)
{
	Timer timer, infoTimer;
	timer.start();
	Move bestMove, ponderMove;
	vector<Move> pvLine;

	for (targetDepth = 1; targetDepth <= maxDepth; targetDepth++) {
		infoTimer.start();
		NegaMax(-oo, oo, targetDepth, 0, forPlayer);
		extractPrincipalVariation(board.hashKey, pvLine, targetDepth, forPlayer);
		infoTimer.stop();
		if (uciInfo) {
			int val = transpositionHash.getValue(board.hashKey);
			cout << "info depth " << targetDepth                                        // Search depth
				 << " score cp " << val;                                                // Score of computer
			if (abs(val) > 10000) {
				cout << " mate " << oo - abs(val);
			}
			cout << " nodes " << nodesVisited                                           // Total visited nodes
				 << " nps " << (int)((double)nodesVisited / (infoTimer.getTime()*1e-6)) // Nodes per second
				 << " time " << infoTimer.getTime()*1e-3                                // Computation time in milliseconds
				 << " pv "; // Principal variation of specified depth
			for (const auto& move : pvLine) cout << move << ' ';
			cout << '\n';
			nodesVisited = 0;
		}

		bestMove = pvLine[0];
		if(targetDepth > 1 && pvLine.size() > 1)
			ponderMove = pvLine[1];
		pvLine.clear();
	}
	timer.stop();
	return pair<Move, Move> (bestMove, ponderMove);
}

int AI::NegaMax(int alpha, int beta, int depth, int ply, color side)
{
	// 4Rnk1/pr3ppp/1p3q2/5NQ1/2p5/8/P4PPP/6K1 w - - 1 0
	// Consistency check:
	// The following FEN should be recognized as Zugzwang. Search should converge after 5 half-moves
	// r4r1k/1pqb1B1p/p3p2B/2bpP2Q/8/1NP5/PP4PP/5R1K w - - 1 0

	int oldAlpha = alpha;
	auto &entry = transpositionHash.getEntry(board.hashKey);
	nodesVisited++;

	// Check if move was already evaluated
	if (entry.terminal == 1) {
		// Checkmate / Stalemate 
		return entry.value;
	}
	if (entry.search_depth >= depth) {
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
		// Evaluate board & reduce Horizon-Effekt
		return QuiescenceSearch(alpha, beta, side);
	}

	MoveList movelist;
	board.updateAllAttacks();
	board.generateMoveList<ALL>(movelist, side);
	sortMoves(movelist, side);
	bool checkedOnThisDepth = board.wasInCheck;
	U64 pinnedOnThisDepth = board.pinned;

	int legalMoves = 0, score = 0;
	Move bestMove;

	// Get first best move (estimate)
	int visited = 0;
	MoveList::iterator move = movelist.begin();
	//ply == targetDepth ? getNextMove<true>(movelist, move, side)
	//	               : getNextMove<false>(movelist, move, side);
	//for (; move != movelist.end(); ) {
	for(auto& move : movelist){
		board.makeMove<FULL>(move, side);

		if (board.isKingLeftInCheck(side, move, checkedOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<FULL>(move, side);
			// Get next best move (estimate)
			//ply == targetDepth ? getNextMove<true>(movelist, move, side)
			//	               : getNextMove<false>(movelist, move, side);
			//move++;
			continue;
		}
		legalMoves++;

		score = -NegaMax(-beta, -alpha, depth - 1, ply + 1, !side);

		board.unMakeMove<FULL>(move, side);

		if (score > alpha) {
			if (score >= beta) {
				return beta; // Beta Cutoff
			}
			alpha = score;
			bestMove = move;
		}
		//ply == targetDepth ? getNextMove<true>(movelist, move, side)
		//	               : getNextMove<false>(movelist, move, side);
		//move++;
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

template<bool firstVisit> void AI::getNextMove(MoveList& mlist, MoveList::iterator& current, color side)
{
	int bestValue = -oo, oldBestValue = -oo;
	MoveList::iterator move = current;
	for (; move != mlist.end(); ++move) {
		oldBestValue = bestValue;
		if (firstVisit) {
			board.makeMove<HASH>(*move, side);
			auto& entry = transpositionHash.getEntry(board.hashKey);
			board.unMakeMove<HASH>(*move, side);

			if (entry.search_depth != -1) {
				// I. PV + Hash moves:
				bestValue = max(bestValue, entry.value);
			}
			else if (move->mtype() == CAPTURE) {
				// II. Captures 
				bestValue = max(bestValue, captureScore[move->movePiece() % 6][move->targetPiece() % 6] - 100000);
			}
			else if (move->mtype() == PAWN2 || (move->mtype() == MOVE && move->movePiece() % 6 == 0)) {
				// III. Pawn pushes
				bestValue = max(bestValue, -1000000);
			}
			if (bestValue != oldBestValue) {
				// Improvement
				iter_swap(current, move);
			}
		}
		else {
			if (move->mtype() == CAPTURE) {
				// II. Captures 
				bestValue = max(bestValue, captureScore[move->movePiece() % 6][move->targetPiece() % 6] - 100000);
			}
			else if (move->mtype() == PAWN2 || (move->mtype() == MOVE && move->movePiece() % 6 == 0)) {
				// III. Pawn pushes
				bestValue = max(bestValue, -1000000);
			}
			if (bestValue != oldBestValue) {
				// Improvement
				iter_swap(current, move);
			}
		}	
	}
}

void inline AI::sortMoves(MoveList& movelist, color side) 
{
	stable_sort(movelist.begin(), movelist.end(), [&, this](const Move& m1, const Move& m2) {
		board.makeMove<HASH>(m1, side);
		auto& e1 = transpositionHash.getEntry(board.hashKey);
		board.unMakeMove<HASH>(m1, side);
		board.makeMove<HASH>(m2, side);
		auto& e2 = transpositionHash.getEntry(board.hashKey);
		board.unMakeMove<HASH>(m2, side);
		int val1 = 0, val2 = 0;
		if (e1.search_depth != -1) {
			val1 = e1.value;
		}
		else if(m1.mtype() == CAPTURE){
			val1 = captureScore[m1.movePiece() % 6][m1.targetPiece() % 6] - 100000;
		}
		if (e2.search_depth != -1) {
			val2 = e2.value;
		}
		else if (m2.mtype() == CAPTURE) {
			val2 = captureScore[m2.movePiece() % 6][m2.targetPiece() % 6] - 100000;
		}
		return val1 < val2;
	});
}

int AI::QuiescenceSearch(int alpha, int beta, color side)
{
	// Plays all (relevant) captures from current position
	// Reduces Horizon Effect
	nodesVisited++;
	int standingPat = board.evaluate(side);
	if (standingPat >= beta)
		return beta;
	if (alpha < standingPat)
		alpha = standingPat;

	MoveList mlist;
	board.updateAllAttacks();
	board.generateMoveList<CAPTURES_ONLY>(mlist, side);

	// MVA-LLV scheme (search best captures first)
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

		int score = -QuiescenceSearch(-beta, -alpha, !side);
		board.unMakeMove<FULL>(capture, side);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

void AI::extractPrincipalVariation(const U64& key, vector<Move>& pvLine, int maxPrintDepth, color side)
{
	const auto& entry = pvTable[key];
	if (maxPrintDepth == 0 || entry.bestmove.invalid()) return;
	board.makeMove<FULL>(entry.bestmove, side);
	pvLine.push_back(entry.bestmove);
	extractPrincipalVariation(board.hashKey, pvLine, maxPrintDepth - 1, !side);
	board.unMakeMove<FULL>(entry.bestmove, side);
}

void AI::reset()
{
	// Time consuming. Only call when ucinewgame is passed to engine
	transpositionHash.clear();
	pvTable.clear();
	board.setupBoard("*");
	aiColor = black;
}

void AI::setFen(string fenstring)
{
	board.setupBoard(fenstring);
}

void AI::printAscii()
{
	board.print();
}

void AI::playStringMoves(const vector<string>& moves, color side)
{
	board.playStringMoves(moves, side);
	sideToMove = moves.size() % 2 == 0 ? sideToMove : !sideToMove;
}

void AI::resetHash()
{
	// Only use when "ucinewgame" is passed
	transpositionHash.clear();
	pvTable.clear();
}

bool AI::isUserMoveValid(const string& usermove, color side)
{
	MoveList possibleMoves;
	board.generateMoveList<ALL>(possibleMoves, side);
	return any_of(possibleMoves.begin(), possibleMoves.end(), [&, this](const Move& move) {
		return shortNotation(move) == usermove;
	}) ? true : false;
}