#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <string>
#include <chrono>
#include <boost/algorithm/string.hpp>

#include "Move.h"
#include "Data.h"
#include "ZobristHash.h"

using namespace std;

typedef vector<Move> MoveList;

class Board
{
	friend class UnitTest;
private:
	// Move making
	void updateAttack(piece p);
	
	// Init
	void initHash(); // Used only for Init!, the hashkey is updated for each move

	// Flooding algorithm
	enum dir {n, e, s, w, ne, se, sw, nw};
	U64 inline floodFill(U64 propagator, U64 empty, dir) const;
	
	void pawnFill(color side);
public:
	Board();
	Board(string fen);
	// Init
	void debug();
	void setupBoard(string FEN);

	// Move making and move generation
	void generateMoveList(MoveList&, color, bool addQuietMoves);

	U64 inline   rookAttacks(long pos, const U64 blockers) const;
	U64 inline bishopAttacks(long pos, const U64 blockers) const;

	void inline pawnMoves(MoveList&, U64 pieces, color, piece, bool addQuietMoves) const;
	void inline knightMoves(MoveList&, U64 pieces, color, piece, bool addQuietMoves) const;
	void inline queen_and_bishopMoves(MoveList&, U64 pieces, const vector<U64>& pattern, color, piece, bool addQuietMoves) const;
	void inline kingMoves(MoveList&, U64 pieces, color, piece king, bool addQuietMoves) const;
	void inline rookMoves(MoveList&, U64 pieces, color, piece, bool addQuietMoves) const;

	void makeMove(const Move&, color side);
	void unMakeMove(const Move&, color side);
	void updateAllAttacks();
	void updatePinnedPieces(color side);
	bool isKingInCheck(color kingColor) const;
	bool isKingLeftInCheck(color KingColor, const Move& lastMove, bool, U64 currentlyPinned);

	// Evaluation
	int evaluate(color side);

	// Misc
	void print() const;

	// Raw data: (public for easy access)
	vector<U64> pieces, attacks;
	byte castlingRights, // Castling rights
		b_enpassent,     // Possible e.p. squares for black
		w_enpassent;     // Possible e.p. squares for white

	U64 wpMove, bpMove,             // Squares pawns can move to (quietly)
		wpDanger, bpDanger,         // Empty squares, that are attacked by pawns
		whitePos, blackPos, allPos, // Collective positional information
		whiteAtt, blackAtt,         // Collective attack information
		hashKey,                    // Hashkey representing current board
		pinned;                     // Squares containing absolute pinned pieces

	ZobristHash hash;
	bool wasInCheck;
	vector<vector<U64>> randomSet;
};

#endif
