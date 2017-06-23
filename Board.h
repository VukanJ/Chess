#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <fstream>
#include <functional>
#include <array>
#include <string>
#include <chrono>
#include <boost/algorithm/string.hpp>

#include "Move.h"
#include "Data.h"
#include "ZobristHash.h"

using namespace std;

typedef vector<Move> MoveList;
enum makeMoveType { PROPER = 0x7, HASH_AND_POS = 0x4, HASH_ONLY = 0x1, POS_ONLY = 0x2 };
// TODO: Test if these correctly influence move-making
enum moveGenType { QUIET_ONLY = 0x1, CAPTURES_ONLY = 0x2, ALL = 0x3};

class Board
{
	friend class UnitTest;
private:
	// Move making
	void updateAttack(piece p);

	// Init
	void initHash(); // Used only for Init!, the hashkey is updated for each move

	// Flooding algorithm
	U64 inline floodFill(U64 propagator, U64 empty, int) const;

	void pawnFill(color side);
public:
	Board();
	Board(string fen);
	// Init
	void debug();
	void setupBoard(string FEN);

	// Move making and move generation
	void generateMoveList(MoveList&, color, bool addQuietMoves);

	array<array<MoveList, 100>, 12> deepMoves;
	array<int, 12> moveUpdateDepths;
	void initDeepMoves();
	void popDeepMovesAtDepth(int depth);
	void updateDeepMoves(int depth, color side, const Move& lastMove);
	void debugDiffDeepMoves(int depth, ofstream*);
	MoveList assembleMovelist(int depth, color side);

	U64 inline   rookAttacks(long pos) const;
	U64 inline bishopAttacks(long pos) const;
	// Vector of all move generating functions
	vector<void(Board::*)(MoveList&)const> moveGenFunction;

	template<moveGenType, color> void inline pawnMoves(MoveList&) const;
	template<moveGenType, color> void inline knightMoves(MoveList&) const;
	template<moveGenType, color, bool Q> void inline queen_and_bishopMoves(MoveList&) const;
	template<moveGenType, color> void inline kingMoves(MoveList&) const;
	template<moveGenType, color> void inline rookMoves(MoveList&) const;

	template<makeMoveType mmt> void makeMove(const Move&, color side);
	template<makeMoveType mmt> void unMakeMove(const Move&, color side);

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
// Template definitions
#include "Makemove.h"
#include "Movegenerators.h"

#endif
