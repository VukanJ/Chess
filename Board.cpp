#include "Board.h"

Board::Board()
	: whitePos(0x0), blackPos(0x0), whiteAtt(0x0), blackAtt(0x0), hashKey(0x0),
	castlingRights(0x0), b_enpassent(0x0), w_enpassent(0x0)
{
	pieces    = vector<U64>(12, 0x0);
	attacks   = vector<U64>(12, 0x0);
}

Board::Board(string fen) : Board()
{
	setupBoard(fen);
	//debug();
}

void Board::setupBoard(string FEN)
{
	for (auto& p : pieces)  p = 0x0;
	for (auto& a : attacks) a = 0x0;
	blackPos = whitePos
		 = bpMove = wpMove = b_enpassent = w_enpassent = castlingRights = 0x0;
	wasInCheck = false;

	// Sets up Board according to FEN
	// FEN = [position(white's perspective) sideToMove castlingrights enpassentSquares NofHalfMoves MoveNumber]
	if (FEN[0] == '*') { // Standard starting position
		for (int i = 0; i < 12; i++)
			pieces[i] = standardPosition[i];
		blackPos = 0xFFFF000000000000ull;
		whitePos = 0xFFFFull;
		allPos = blackPos | whitePos;
		castlingRights = 0xFull;
	}
	else {
		boost::trim(FEN);
		vector<string> fenArgs = { "" };
		for (auto it = FEN.begin() + 1; it != FEN.end();) {
			if (*it == ' ' && *(it - 1) == ' ')
				it = FEN.erase(it);
			else it++;
		}
		for (auto f : FEN) {
			if (f == ' ')
				fenArgs.push_back("");
			else fenArgs.back().push_back(f);
		}
		if (fenArgs.size() != 6) {
			cerr << "Invalid FEN!\n";
			exit(1);
		}
		int counter = -1;
		for (auto& f : fenArgs[0]) {
			if (isdigit(f))
				counter += f - 48;
			else {
				if (f != '/') {
					counter++;
					pieces[getPieceIndex(f)] |= bit_at_rev(counter);
				}
			}
		}
		for_black(p) blackPos |= pieces[p];
		for_white(p) whitePos |= pieces[p];
		// Set castling rights
		for (auto c : fenArgs[2]) {
			switch (c) {
			case 'k': castlingRights |= castle_k; break;
			case 'K': castlingRights |= castle_K; break;
			case 'q': castlingRights |= castle_q; break;
			case 'Q': castlingRights |= castle_Q; break;
			}
		}
		allPos = blackPos | whitePos;
	}
	bpDanger = wpDanger = pinned = 0x0;
	updateAllAttacks();
	initHash();
}

void Board::initHash()
{
	// Generates initial hashkey.
	// Generates constant table of random 64Bit numbers.
	random_device r_device;
	mt19937_64 generator(r_device());
	//generator.seed(42);
	uniform_int_distribution<U64> distr;
	randomSet = vector<vector<U64>>(14, vector<U64>(64, 0));
	// Index 0-11: Piece type
	for (auto& r1 : randomSet)
		for (auto& r2 : r1)
			r2 = distr(generator);
	randomSet.push_back(vector<U64>(4, 0x0));

	// The following calculations do not need to be repeated when castling in game
	randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_k] = randomSet[bk][e8]
		                                           ^ randomSet[bk][g8]
		                                           ^ randomSet[br][h8]
		                                           ^ randomSet[br][f8];
	randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_q] = randomSet[bk][e8]
		                                           ^ randomSet[bk][c8]
		                                           ^ randomSet[br][a8]
		                                           ^ randomSet[br][d8];
	randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_K] = randomSet[wk][e1]
		                                           ^ randomSet[wk][g1]
		                                           ^ randomSet[wr][h1]
		                                           ^ randomSet[wr][f1];
	randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q] = randomSet[wk][e1]
		                                           ^ randomSet[wk][c1]
		                                           ^ randomSet[wr][a1]
		                                           ^ randomSet[wr][d1];

	auto pos = 0, i = 0;
	for (auto p : pieces) {
		for_bits(pos, p) {
			hashKey ^= randomSet[i][pos];
		}
		i++;
	}
	hashKey ^= randomSet[CASTLE_HASH][castlingRights];
}

void Board::debug()
{
	//cout << "Castling rights -> ";
	//printBits(castlingRights);

	// debug
	//printf("blocked black pawns: %d\n", blockedPawn(black));
	//printf("blocked white pawns: %d\n", blockedPawn(white));
	auto startingHash = hashKey;
	MoveList movelist;
	cout << "Board value (w): " << evaluate(white) << endl;
	movelist.clear();
	print();

	color debugPlayerColor = black;

	generateMoveList(movelist, debugPlayerColor, true);

	cout << "Start hash " << hex << hashKey << endl;
	//int count = 0;
	//for (auto& m : movelist) {
	//	cout << moveString(m) << (count % 10 == 0 ? "\n" : "  ");
	//	count++;
	//	makeMove(m, sideToMove);
	//	//print();
	//	//printBitboard(whitePos);
	//	unMakeMove(m, sideToMove);
	//	//print();
	//	//printBitboard(whitePos);
	//}

	cout << "\nEnd hash   " << hex << hashKey << endl;

	if (startingHash != hashKey) {
		cerr << "\t\t\t::: HASHING ERROR :::\n";
		printBitboard(hashKey);
	}
	else clog << "::: HASH OK :::\n";
	if (!pieces[bk] || !pieces[wk])
		cerr << "Missing Kings!!\n";
	if (popcount(pieces[bk]) > 1 || popcount(pieces[wk]) > 1) {
		// No need to handle multiple kings
		cerr << "Too many kings. Invalid Board!\n" << endl;
		exit(1);
	}
}

void Board::updateAllAttacks()
{
	whiteAtt = blackAtt = 0x0;
	for_black(i){
		 // Generate only if bitboard not empty
		if(pieces[i])
			updateAttack((piece)i);
		else
			attacks[i] = 0x0;
		attacks[i] &= ~blackPos;
		blackAtt   |= attacks[i];
	}
	for_white(i) {
		 // Generate only if bitboard not empty
		if(pieces[i])
			updateAttack((piece)i);
		else
			attacks[i] = 0x0;
		attacks[i] &= ~whitePos;
		whiteAtt   |= attacks[i];
	}
}

void Board::updateAttack(piece p)
{
	// Fill all the bits that are attacked by individual pieces
	// including attacked enemy pieces. This method is only invoked
	// for non empty pieces

	ulong pos = -1;
	U64 mask = 0;
	switch (p){
		case bp:
			pawnFill(black);
			break;
		case wp:
			pawnFill(white);
			break;
		case br: case wr:
			attacks[p] = 0x0;
			mask = pieces[p];
			for_bits(pos, mask) attacks[p] |= rookAttacks(pos, allPos);
			break;
		case bn:
			mask = pieces[bn];
			attacks[bn] = 0x0;
			for_bits(pos,mask) attacks[bn] |= KNIGHT_ATTACKS[pos] & ~blackPos;
			break;
		case wn:
			mask = pieces[wn];
			attacks[wn] = 0x0;
			for_bits(pos, mask) attacks[wn] |= KNIGHT_ATTACKS[pos] & ~whitePos;
			break;
		case bb: case wb:
			attacks[p] = 0x0;
			mask = pieces[p];
			for_bits(pos, mask) attacks[p] |= bishopAttacks(pos, allPos);
			break;
		case bk:
			mask = pieces[bk];
			attacks[bk] = 0x0;
			bitScan_rev64(pos, mask);
			attacks[bk] |= KING_ATTACKS[pos] & ~blackPos;
			break;
		case wk:
			mask = pieces[wk];
			attacks[wk] = 0x0;
			bitScan_rev64(pos, mask);
			attacks[wk] |= KING_ATTACKS[pos] & ~whitePos;
			break;
		case bq: case wq:
			attacks[p] = 0x0;
			mask = pieces[p];
			for_bits(pos, mask) attacks[p] |= (rookAttacks(pos, allPos) | bishopAttacks(pos, allPos));
			break;
	}
}

U64 inline Board::floodFill(U64 propagator, U64 empty, dir direction) const
{
	// Calculates all attacks including attacked pieces for sliding pieces
	// (Queen, Rook, bishop)(s)
	// Not used in-game
	U64 flood = propagator;
	U64 wrap = noWrap[direction];
	empty &= wrap;
	auto r_shift = shift[direction];
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= rotate_l64(propagator, r_shift) & empty;
	return rotate_l64(flood, r_shift) & wrap;
}

void Board::pawnFill(color side)
{
	if (side == black){
		attacks[bp] = bpMove = 0x0;
		// Normal step
		bpMove |= (pieces[bp] >> 8) & ~allPos;
		// Double step
		bpMove |= ((((pieces[bp] & 0xFF000000000000ull) >> 8) & ~allPos) >> 8) & ~allPos;
		// Side Attacks
		bpDanger  = ((pieces[bp] & ~_right) >> 9) | ((pieces[bp] & ~_left) >> 7);
		attacks[bp] |= whitePos & bpDanger;
		bpDanger &= ~whitePos;
	}
	else{
		attacks[wp] = wpMove = 0x0;
		// Normal step
		wpMove |= (pieces[wp] << 8) & ~allPos;
		// Double step
		wpMove |= ((((pieces[wp] & 0xFF00ull) << 8) & ~allPos) << 8) & ~allPos;
		// Side Attacks
		wpDanger = ((pieces[wp] & ~_left) << 9) | ((pieces[wp] & ~_right) << 7);
		attacks[wp] |= blackPos & wpDanger;
		wpDanger &= ~blackPos;
	}
}

void Board::updatePinnedPieces(color side)
{
	// Calculates absolute pinned pieces.
	// These are considered when checking move legality

	U64 kingXray = 0x0, kingRects = 0x0;
	U64 xray = 0x0;
	pinned = 0x0;
	//print();
	int kingPosition;
	if (side == white) {
		kingPosition = msb(pieces[wk]);
		kingXray = BISHOP_ATTACKS[kingPosition] & (pieces[bq] | pieces[bb]);
		kingXray |= ((_col << kingPosition % 8) ^ (_row << (kingPosition / 8) * 8)) & (pieces[bq] | pieces[br]);
		//printBitboard(kingXray);
		// Find positions of pieces, that must not be moved
		for_bits(enemyPos, kingXray) {
			xray = CONNECTIONS[enemyPos][kingPosition] & allPos;
			if (popcount(xray) == 1 && popcount(xray & whitePos) == 1) {
				pinned |= xray & whitePos;
			}
		}
	}
	else {
		kingPosition = msb(pieces[bk]);
		kingXray = BISHOP_ATTACKS[kingPosition] & (pieces[wq] | pieces[wb]);
		kingXray |= ((_col << kingPosition % 8) ^ (_row << (kingPosition / 8) * 8)) & (pieces[wq] | pieces[wr]);
		//printBitboard(kingXray);
		// Find positions of pieces, that must not be moved
		for_bits(enemyPos, kingXray) {
			xray = CONNECTIONS[enemyPos][kingPosition] & allPos;
			if (popcount(xray) == 1 && popcount(xray & blackPos) == 1) {
				pinned |= xray & blackPos;
			}
		}
	}
	//printBitboard(pinned);
}

void Board::generateMoveList(MoveList & moveList, color side, bool addQuietMoves)
{
	/*
	This method generates a list of all possible moves for a player.
	Aggressive and special moves are generated first and preferably stored
	at the front of the movelist
	*/

	ulong pos = nullSquare;
	U64 attackMask = 0x0;
	U64 pieceAttacks = 0x0, attackingPieces = 0x0;
	// Generate all capturing and normal moves
	updatePinnedPieces(side);
	if (side == black){
		/*
		   ::::::::::::::::::::::::::::::::::
		   ::     BLACK MOVE GENERATION    ::
		   ::::::::::::::::::::::::::::::::::
		*/
		wasInCheck = (pieces[bk] & whiteAtt) > 0;
		for_black(b) { // Loop through black pieces
			attackingPieces = pieces[b];
			if (attackingPieces) { // Only consider non-empty boards
				switch (b) {
				case bp: pawnMoves<ALL, black>(moveList); break;
				case br: rookMoves<ALL, black>(moveList); break;
				case bn: knightMoves<ALL, black>(moveList); break;
				case bb: queen_and_bishopMoves<ALL, black, false>(moveList); break;
				case bq: queen_and_bishopMoves<ALL, black, true>(moveList); break;
				case bk: kingMoves<ALL, black>(moveList); break;
				}
			}
		}
		// Generate castling moves
		// Black King can castle if there are no pieces between king and rook, both havent moved yet and king
		// does not cross attacked squares during castling, same for white
		if (castlingRights & castle_k && !(allPos & 0x600000000000000ull) && !(whiteAtt & 0xE00000000000000ull)) {
			moveList.emplace_back(castlingRights, BCASTLE);
		}
		if (castlingRights & castle_q && !(allPos & 0x7000000000000000ull) && !(whiteAtt & 0x3800000000000000ull)) { // Black King can castle (big)
			moveList.emplace_back(castlingRights, BCASTLE_2);
		}
	}
	else{
		/*
			::::::::::::::::::::::::::::::::::
			::     WHITE MOVE GENERATION    ::
			::::::::::::::::::::::::::::::::::
		*/
		wasInCheck = (pieces[wk] & blackAtt) > 0;
		for_white(w) { // Loop through white pieces
			attackingPieces = pieces[w];
			if (attackingPieces){
				switch (w) {
				case wp: pawnMoves<ALL, white>(moveList); break;
				case wr: rookMoves<ALL, white>(moveList); break;
				case wn: knightMoves<ALL, white>(moveList); break;
				case wb: queen_and_bishopMoves<ALL, white, false>(moveList); break;
				case wq: queen_and_bishopMoves<ALL, white, true>(moveList); break;
				case wk: kingMoves<ALL, white>(moveList); break;
				}
			}
		}
		 //  Castling permission King-rook path is not obstructed and not under attack
		if (castlingRights & castle_K && !(allPos & 0x6ull) && !(blackAtt & 0xEull)) { // White King can castle
			moveList.emplace_back(castlingRights, WCASTLE);
		}
		if (castlingRights & castle_Q && !(allPos & 0x70ull) && !(blackAtt & 0x38ull)) { // White King can castle queenside
			moveList.emplace_back(castlingRights, WCASTLE_2);
		}
	}
	// If opponent rook has been captured, he looses castling rights.
	// TODO: Needs nicer solution

	//if (isKingInCheck(side))
	//	reduceMoveList(moveList, side);

	for_each(moveList.begin(), moveList.end(), [this](Move& move) {
		if (target_piece(move.pieces) == wr) {
			if (move_type(move.flags) == CAPTURE || move_type(move.flags) == C_PROMOTION) {
				if (move.to == a1)
					move.flags |= (castlingRights & castle_Q) << 4;
				else if (move.to == h1)
					move.flags |= (castlingRights & castle_K) << 4;
			}
		}
		else if (target_piece(move.pieces) == br) {
			if (move_type(move.flags) == CAPTURE || move_type(move.flags) == C_PROMOTION) {
				if (move.to == a8)
					move.flags |= (castlingRights & castle_q) << 4;
				else if (move.to == h8)
					move.flags |= (castlingRights & castle_k) << 4;
			}
		}
	});
}

U64 inline Board::rookAttacks(long pos, U64 blockers) const
{
	// Calculate attack set with magic database
	U64 index = ((rookAttackMasks[pos] & blockers) * rookMagics[pos]) >> rookMagicShifts[pos];
	return magicRookMoveDatabase[pos][index];
}

U64 inline Board::bishopAttacks(long pos, U64 blockers) const
{
	// Calculate attack set with magic database
	U64 index = ((bishopAttackMasks[pos] & blockers) * bishopMagics[pos]) >> bishopMagicShifts[pos];
	return magicBishopMoveDatabase[pos][index];
}

bool Board::isKingInCheck(color kingColor) const
{
	// Returns true if king is in check. Requires accurate attack sets.
	return kingColor == black ? (pieces[bk] & whiteAtt) != 0 : (pieces[wk] & blackAtt) != 0;
}

bool Board::isKingLeftInCheck(color kingColor, const Move& lastMove, bool wasCheck, U64 currentlyPinned)
{
	// Returns true if last played move leaves king in check.
	// Only relies on positional information

	if (   wasCheck
		|| move_piece(lastMove.pieces) == bk
		|| move_piece(lastMove.pieces) == wk
		|| (bit_at(lastMove.from) & currentlyPinned)) {

		// King was in check before last move, or king was moved, or moved piece was pinned
		// Check if king is in check now

		piece king = kingColor == white ? wk : bk;
		byte kingPos = msb(pieces[king]);
		U64 kingRect = 0x0, kingDiags = 0x0;
		if (move_type(lastMove.flags) > 5) {
			return false; // Castling does not put king in check
		}

		if (kingColor == white) {
			// Check if last move was quiet and from square did not lie on ray attack:
			if (KNIGHT_ATTACKS[kingPos] & pieces[bn]) return true; // King attacked by opponent knight
			if (KING_ATTACKS[kingPos] & pieces[bk]) return true; // King attacked by opponent king

			if ((0x5ull << ((kingPos - 1) + 8))
				& (_row << 8 * ((kingPos / 8) + 1))
				& pieces[bp]) return true; // King attacked by opponent pawns
		}
		else {
			if (KNIGHT_ATTACKS[kingPos] & pieces[wn]) return true; // King attacked by opponent knight
			if (KING_ATTACKS[kingPos] & pieces[wk]) return true; // King attacked by opponent king

			if ((0x5ull << ((kingPos - 1) + -8))
				& (_row << 8 * ((kingPos / 8) - 1))
				& pieces[wp]) return true; // King attacked by opponent pawns
		}

		// Check if enemy attack was uncovered by lastMove

		if (kingColor == white) {
			kingDiags |= bishopAttacks(kingPos, allPos);
			if ((kingDiags) & (pieces[bq] | pieces[bb])) return true;
			kingRect |= rookAttacks(kingPos, allPos);
			if ((kingRect)  & (pieces[br] | pieces[bq])) return true;
		}
		else {
			kingDiags |= bishopAttacks(kingPos, allPos);
			if ((kingDiags) & (pieces[wq] | pieces[wb])) return true;
			kingRect |= rookAttacks(kingPos, allPos);
			if ((kingRect)  & (pieces[wr] | pieces[wq])) return true;
		}
		return false;  // Move is legal
	}
	else return false; // Move is legal
}

int Board::evaluate(color side)
{
	// Returns the relative heuristic value of the board for the white player
	// Score of black is the negative of white's score

	// Endgamevalue determined by minimum number of piecer per player
	float endGameValue = 1 - 1.0f / 16.0f * min(popcount(whitePos), popcount(blackPos));
	int total_boardValue = 0;

	// *************************** MATERIAL ***************************
	/* Material values in units of centipawns (cp):
	 * Queen  -> 900 cp
	 * Rook   -> 500 cp
	 * Bishop -> 300 cp
	 * Knight -> 300 cp
	 * Pawn   -> 100 cp
	 * King   ->   0 cp
	 */
	total_boardValue += 900 * (popcount(pieces[wq]) - popcount(pieces[bq]))
		              + 500 * (popcount(pieces[wr]) - popcount(pieces[br]))
		              + 300 * (popcount(pieces[wb]) - popcount(pieces[bb]))
		              + 300 * (popcount(pieces[wn]) - popcount(pieces[bn]))
		              + 100 * (popcount(pieces[wp]) - popcount(pieces[bp]));

	// *************************** POSITION ***************************

	//// Rewards points, if positions are similar to piece-square-heuristics
	//// Pawns:
	int psh = 0;
	U64 mask = pieces[wp];
	//for_bits(pos, mask)
	//	psh += pieceSquareTable[0][63 - pos];
	//mask = pieces[bp];
	//for_bits(pos, mask)
	//	psh -= pieceSquareTable[0][pos];
	//mask = pieces[wn];
	//for_bits(pos, mask)
	//	psh += pieceSquareTable[1][63 - pos];
	//mask = pieces[bn];
	//for_bits(pos, mask)
	//	psh -= pieceSquareTable[1][pos];
	//mask = pieces[wb];
	//for_bits(pos, mask)
	//	psh += pieceSquareTable[2][63 - pos];
	//mask = pieces[bb];
	//for_bits(pos, mask)
	//	psh -= pieceSquareTable[2][pos];
	//
	//if (endGameValue > 0.8) {
	//	mask = pieces[wk];
	//	for_bits(pos, mask)
	//		psh += pieceSquareTable[4][63 - pos];
	//	mask = pieces[bk];
	//	for_bits(pos, mask)
	//		psh -= pieceSquareTable[4][pos];
	//}
	//else {
	//	mask = pieces[wk];
	//	for_bits(pos, mask)
	//		psh += pieceSquareTable[3][63 - pos];
	//	mask = pieces[bk];
	//	for_bits(pos, mask)
	//		psh -= pieceSquareTable[3][pos];
	//}
	//total_boardValue += psh / 10;

	// *************************** MOBILITY ***************************
	// Determines how many squares are accessible, worth 10 cp each
	int mobility = 0;
	for_white(i) mobility += popcount(attacks[i] ^ blackPos);
	for_black(i) mobility -= popcount(attacks[i] ^ whitePos);
	total_boardValue += mobility * 5;
	mobility = 0;
	// Measure "hostiliy" = number of attacked pieces of opponent. 15cp each
	for_white(i) mobility += popcount(attacks[i] & blackPos);
	for_black(i) mobility += popcount(attacks[i] & whitePos);
	total_boardValue += mobility * 10;
	// ~~~ Blocked Pawns ~~~
	// Determines how many pawns are blocked per player color, penalty of 4 cp for each
	total_boardValue += 4 * (popcount((pieces[bp] >> 8) & allPos)
						   - popcount((pieces[wp] << 8) & allPos));

	//*************************** KING SAFETY ***************************
	// Penalty of 250 cp if king is in check, since it generally
	// reduces number of possible moves.
	if (pieces[bk] & whiteAtt)      total_boardValue += 250;
	else if (pieces[wk] & blackAtt) total_boardValue -= 250;
	// ~~~ King freedom ~~~
	// Measures number of fields the king can escape to. This should only be
	// active in the endgame -> Leads to a quicker checkmate and less transpositions
	if (endGameValue > 0.5) {
		//cout << "OK\n";
		mask = pieces[wk];
		mask |= rookAttacks(msb(mask), allPos) | bishopAttacks(msb(mask), allPos);

		total_boardValue += 10 * popcount(mask & ~blackAtt);
		mask = pieces[bk];
		mask |= rookAttacks(msb(mask), allPos) | bishopAttacks(msb(mask), allPos);
		total_boardValue -= 10 * popcount(mask & ~whiteAtt);
	}

	// Pawn shield. Count number of pawns in front of kings in a 2x3 area 0x707
	// Kings on the edges are not being rewarded points

	total_boardValue += 10*popcount((0x707ull >> (msb(pieces[wk] & _noSides) - 3)) & pieces[wp]);
	total_boardValue -= 10*popcount((0x707ull << (msb(pieces[bk] & _noSides) + 3)) & pieces[bp]);

	return (side == white ? 1 : -1) * total_boardValue;
}

void Board::print() const
{
	// Print full chessboard with symbols and borders without attacked squares
	vector<string> asciiBoard = vector<string>(8, string(8, '.'));
	for (int p = 0; p < 12; p++) {
		auto temp = pieces[p];
		auto count = -1;
		for (U64 b = _msb; b != 0; b >>= 1) {
			count++;
			if (b & temp) asciiBoard[count / 8][count % 8] = names[p];
		}
	}
#ifdef _WIN32 // Since Unicode is not really supported in C++ yet
	cout << string(10, (char)(219)) << endl;
	for (auto r : asciiBoard) {
		cout << char(219);
		for (auto c : r) cout << c;
		cout << char(219) << '\n';
	}
	cout << string(10, (char)(219)) << '\n';
#else
	auto repChar = [](int c) {for (int i = 0; i < c; i++)cout << "\u2588"; };
	repChar(10); cout << '\n';
	for (auto r : asciiBoard) {
		cout << "\u2588";
		for (auto c : r) cout << c;
		cout << "\u2588" << '\n';
	}
	repChar(10); cout << "\n\n";
#endif
}
