#include "AI.h"

AI::AI(string FEN, color computerColor)
	: aiColor(computerColor)
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

void AI::bindGui(Gui* guiPtr)
{
	gui = guiPtr;
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

pair<Move, Move> AI::getBestMove(color forPlayer, int maxDepth)
{
	Move bestMove, ponderMove;
	vector<Move> pvLine;
	for (targetDepth = 1; targetDepth < maxDepth; targetDepth++) {
		NegaMax(-oo, oo, targetDepth, 0, forPlayer);
		extractPrincipalVariation(board.hashKey, pvLine, targetDepth, forPlayer);
		bestMove = pvLine[0];
		if(targetDepth > 1)
			ponderMove = pvLine[1];
		pvLine.clear();
	}
	return pair<Move, Move> (bestMove, ponderMove);
}

int AI::NegaMax(int alpha, int beta, int depth, int ply, color side)
{
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
		board.updateAllAttacks();
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

		if (board.isKingLeftInCheck(side, move, checkedOnThisDepth, pinnedOnThisDepth)) {
			board.unMakeMove<FULL>(move, side);
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

int AI::QuiescenceSearch(int alpha, int beta, int ply, color side)
{
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

		int score = -QuiescenceSearch(-beta, -alpha, ply + 1, !side);
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