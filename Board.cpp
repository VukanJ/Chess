#include "Board.h"

Board::Board()
	: whitePos(0x0), blackPos(0x0), whiteAtt(0x0), blackAtt(0x0), hashKey(0x0),
	castlingRights(0x0), b_enpassent(0x0), w_enpassent(0x0)
{
	pieces    = vector<u64>(12, 0x0);
	attacks   = vector<u64>(12, 0x0);
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
	uniform_int_distribution<u64> distr;
	randomSet = vector<vector<u64>>(14, vector<u64>(64, 0));
	// Index 0-11: Piece type
	for (auto& r1 : randomSet)
		for (auto& r2 : r1)
			r2 = distr(generator);
	randomSet.push_back(vector<u64>(4, 0x0));

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
		BITLOOP(pos, p) {
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

	generateMoveList(movelist, debugPlayerColor);

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
	u64 mask = 0;
	switch (p){
		case bp:
			pawnFill(black);
			break;
		case wp:
			pawnFill(white);
			break;
		case br: case wr:
			attacks[p] = 0x0;
			for (int i = 0; i < 4; ++i)
				attacks[p] |= floodFill(pieces[p], ~allPos, (dir)i);
			break;
		case bn:
			mask = pieces[bn];
			attacks[bn] = 0x0;
			BITLOOP(pos,mask) attacks[bn] |= KNIGHT_ATTACKS[pos] & ~blackPos;
			break;
		case wn:
			mask = pieces[wn];
			attacks[wn] = 0x0;
			BITLOOP(pos, mask) attacks[wn] |= KNIGHT_ATTACKS[pos] & ~whitePos;
			break;
		case bb: case wb:
			attacks[p] = 0x0;
			for (int i = 4; i < 8; ++i)
				attacks[p] |= floodFill(pieces[p], ~allPos, (dir)i);
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
			for (int i = 0; i < 8; ++i)
				attacks[p] |= floodFill(pieces[p], ~allPos, (dir)i);
			break;
	}
}

u64 Board::floodFill(u64 propagator, u64 empty, dir direction) const
{
	// Calculates all attacks including attacked pieces for sliding pieces
	// (Queen, Rook, bishop)(s)
	// TODO: This method is very performance intensive. Rewrite!

	u64 flood = propagator;
	u64 wrap = noWrap[direction];
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
		attacks[bp] |= whitePos & ((pieces[bp] & ~_right) >> 9);
		attacks[bp] |= whitePos & ((pieces[bp] & ~_left)  >> 7);
	}
	else{
		attacks[wp] = wpMove = 0x0;
		// Normal step
		wpMove |= (pieces[wp] << 8) & ~allPos;
		// Double step
		wpMove |= ((((pieces[wp] & 0xFF00ull) << 8) & ~allPos) << 8) & ~allPos;
		// Side Attacks
		attacks[wp] |= blackPos & ((pieces[wp] & ~_left)  << 9);
		attacks[wp] |= blackPos & ((pieces[wp] & ~_right) << 7);
	}
}

void inline Board::pawnMoves(MoveList& moveList, u64 attackingPieces, color side, piece pawn) const
{
	u64 attackMask = 0x0, pieceAttacks = 0x0;
	if(side == black){
		// Find normal captures:
		BITLOOP(pos, attackingPieces) {
			attackMask = ((bit_at(pos) >> 9) & ~_left)  & whitePos;
			attackMask |= ((bit_at(pos) >> 7) & ~_right) & whitePos;

			for_white(candidate) {
				pieceAttacks = pieces[candidate] & attackMask;

				BITLOOP(target, pieceAttacks) {
					if (target < 8) {
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, bq)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, bn)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, br)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, bb)));
					}
					else {
						moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(bp, candidate)));
					}
				}

			}
		}
		// Find normal upwards moves and double pawn steps:
		attackingPieces = (pieces[bp] >> 8) & bpMove;
		BITLOOP(pos, attackingPieces) {
			if (pos > 7) {
				moveList.push_back(Move(pos + 8, pos, MOVE, bp));
			}
			else {
				moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, piece_pair(bp, bq)));
				moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, piece_pair(bp, bn)));
				moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, piece_pair(bp, br)));
				moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, piece_pair(bp, bb)));
			}
		}
		// Double pawn move
		attackingPieces = ((((0x00FF000000000000 & pieces[bp]) >> 8) & bpMove) >> 8) & bpMove;
		BITLOOP(pos, attackingPieces) {
			moveList.push_back(Move(pos + 16, pos, PAWN2, bp));
		}
		// Enpassent

		if (b_enpassent) {
			// There surely exists an enpassent move
			if ((bit_at(24 + b_enpassent) & pieces[bp]) & (_row << 24)) {
				// black pawn right of ep square
				moveList.push_back(Move(24 + b_enpassent, 15 + b_enpassent, ENPASSENT, bp));
			}
			if (bit_at(22 + b_enpassent) & pieces[bp] & (_row << 24)) {
				// black pawn left of ep square
				moveList.push_back(Move(22 + b_enpassent, 15 + b_enpassent, ENPASSENT, bp));
			}
		}
	}
	else {
		// Find normal captures:
		// attackingPieces stands for attacked squares in this case
		BITLOOP(pos, attackingPieces) {
			attackMask = (bit_at(pos) << 9 & ~_right) & blackPos;
			attackMask |= (bit_at(pos) << 7 & ~_left)  & blackPos;

			for_black(candidate) {
				pieceAttacks = pieces[candidate] & attackMask;
				BITLOOP(target, pieceAttacks) {
					if (target > 55) {
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, wq)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, wn)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, wr)));
						moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, piece_pair(candidate, wb)));
					}
					else {
						moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(wp, candidate)));
					}
				}
			}
		}
		// Find normal upwards moves and double pawn steps:
		attackingPieces = (pieces[wp] << 8) & wpMove;
		BITLOOP(pos, attackingPieces) {
			if (pos < 56) {
				moveList.push_back(Move(pos - 8, pos, MOVE, wp));
			}
			else {
				moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, piece_pair(wp, wq)));
				moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, piece_pair(wp, wn)));
				moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, piece_pair(wp, wr)));
				moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, piece_pair(wp, wb)));
			}
		}
		attackingPieces = ((((0xFF00 & pieces[wp]) << 8) & wpMove) << 8) & wpMove;
		BITLOOP(pos, attackingPieces) {
			moveList.push_back(Move(pos - 16, pos, PAWN2, wp));
		}
		if (w_enpassent) { 
			// There surely exists an enpassent move
			if ((bit_at(30 + w_enpassent) & pieces[wp]) & (_row << 32)) {
				// white pawn right of ep square
				moveList.push_back(Move(30 + w_enpassent, 39 + w_enpassent, ENPASSENT, wp));
			 }
			 if (bit_at(32 + w_enpassent) & pieces[wp] & (_row << 32)) {
				// white pawn left of ep square
				moveList.push_back(Move(32 + w_enpassent, 39 + w_enpassent, ENPASSENT, wp));
			}
		}
	}
}

void inline Board::knightMoves(MoveList& moveList, u64 attackingPieces, color side, piece p) const
{
	u64 attackMask = 0x0, pieceAttacks = 0x0;

	BITLOOP(pos, attackingPieces) {
		attackMask = KNIGHT_ATTACKS[pos] & attacks[p] & (side == black ? whitePos : blackPos);

		for_color(candidate, !side) {
			pieceAttacks = pieces[candidate] & attackMask;
			if (pieceAttacks) {
				BITLOOP(target, pieceAttacks) {
					moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(p, candidate)));
				}
			}
		}

		attackMask ^= KNIGHT_ATTACKS[pos] & attacks[p];
		BITLOOP(target, attackMask) {
			moveList.push_back(Move(pos, target, MOVE, p));
		}
	}
}

void inline Board::queen_and_bishopMoves(MoveList& moveList, u64 attackingPieces, const vector<u64>& pattern, color side, piece p) const
{
	u64 attackMask = 0x0, pieceAttacks = 0x0;

	BITLOOP(pos, attackingPieces) {
		attackMask = pattern[pos] & attacks[p] & (side == black ? whitePos : blackPos);

		for_color(candidate, !side) {
			pieceAttacks = pieces[candidate] & attackMask;
			if (pieceAttacks) {
				BITLOOP(target, pieceAttacks) {
					if (!(CONNECTIONS[pos][target] & allPos)) {
						moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(p, candidate)));
					}
				}
			}
		}

		attackMask ^= pattern[pos] & attacks[p];
		BITLOOP(target, attackMask) {
			if (!(CONNECTIONS[pos][target] & allPos)) {
				moveList.push_back(Move(pos, target, MOVE, p));
			}
		}
	}
}

void inline Board::kingMoves(MoveList& moveList, u64 attackingPieces, color side, piece king) const
{
	u64 attackMask = 0x0, pieceAttacks = 0x0;
	ulong pos = msb(pieces[king]);
	attackMask = ((attacks[king] & (side == black ? whitePos : blackPos)) & ~(side == black ? whiteAtt : blackAtt));

	for_color(candidate, !side) {
		pieceAttacks = pieces[candidate] & attackMask;
		if (pieceAttacks) {
			BITLOOP(target, pieceAttacks) {
				moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & (side == black ? 0x3 : 0xC)), piece_pair(king, candidate)));
			}
		}
	}

	attackMask ^= (KING_ATTACKS[pos] & attacks[king]) & ~(side == black ? whiteAtt : blackAtt);
	BITLOOP(target, attackMask) {
		moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & (side == black ? 0x3 : 0xC)), king));
	}
}

void inline Board::rookMoves(MoveList& moveList, u64 attackingPieces, color side, piece rook) const
{
	u64 attackMask = 0x0, pieceAttacks = 0x0;
	ulong a_square, h_square, qCastRight, kCastRight;
	if (side == black) {
		a_square = a8;
		h_square = h8;
		qCastRight = castle_q;
		kCastRight = castle_k;
	}
	else {
		a_square = a1;
		h_square = h1;
		qCastRight = castle_Q;
		kCastRight = castle_K;
	}
	// Calculate attacked pieces
	BITLOOP(pos, attackingPieces) {
		attackMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[rook] & (side == black ? whitePos : blackPos);

		if (attackMask) {
			for_color(candidate, !side) {
				pieceAttacks = pieces[candidate] & attackMask;

				BITLOOP(target, pieceAttacks) {
					if (!(CONNECTIONS[pos][target] & allPos)) {
						if (pos == a_square) {
							moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & qCastRight), piece_pair(rook, candidate)));
						}
						else if (pos == h_square) {
							moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & kCastRight), piece_pair(rook, candidate)));
						}
						else {
							moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(rook, candidate)));
						}
					}
				}
			}
		}

		attackMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[rook];
		BITLOOP(target, attackMask) {
			if (!(CONNECTIONS[pos][target] & allPos)) {
				if (pos == a_square) {
					moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & qCastRight), rook));
				}
				else if (pos == h_square) {
					moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & kCastRight), rook));
				}
				else {
					moveList.push_back(Move(pos, target, MOVE, rook));
				}
			}
		}
	}
}

void Board::generateMoveList(MoveList & moveList, color side) const
{
	/*
	This method generates a list of all possible moves for a player.
	Aggressive and special moves are generated first and preferably stored
	at the front of the movelist
	*/
	ulong pos = nullSquare;
	u64 attackMask = 0x0;
	u64 pieceAttacks = 0x0, attackingPieces = 0x0;
	// Generate all capturing and normal moves

	if (side == black){ 
		/* 
		   ::::::::::::::::::::::::::::::::::
		   ::     BLACK MOVE GENERATION    ::
		   ::::::::::::::::::::::::::::::::::
		*/
		for_black(b) { // Loop through black pieces
			attackingPieces = pieces[b];
			if (attackingPieces) { // Only consider non-empty boards
				switch (b) {
				case bp: pawnMoves(moveList, pieces[bp], black, bp); break;
				case br: rookMoves(moveList, pieces[br], black, br); break;
				case bn: knightMoves(moveList, pieces[bn], black, bn); break;
				case bb: queen_and_bishopMoves(moveList, pieces[bb], BISHOP_ATTACKS, black, bb); break;
				case bq: queen_and_bishopMoves(moveList, pieces[bq], QUEEN_ATTACKS,  black, bq); break;
				case bk: kingMoves(moveList, pieces[bk], black, bk); break;
				}
			}
		}
		// Generate castling moves
		// Black King can castle if there are no pieces between king and rook, both havent moved yet and king
		// does not cross attacked squares during castling, same for white
		if (castlingRights & castle_k && !(allPos & 0x600000000000000ull) && !(whiteAtt & 0xE00000000000000ull)) {
			moveList.push_back(Move(castlingRights, BCASTLE));
		}
		if (castlingRights & castle_q && !(allPos & 0x7000000000000000ull) && !(whiteAtt & 0x3800000000000000ull)) { // Black King can castle (big)
			moveList.push_back(Move(castlingRights, BCASTLE_2));
		}
	}
	else{
		/*
			::::::::::::::::::::::::::::::::::
			::     WHITE MOVE GENERATION    ::
			::::::::::::::::::::::::::::::::::
		*/
		for_white(w) { // Loop through white pieces
			attackingPieces = pieces[w];
			if (attackingPieces){
				switch (w) {
				case wp: pawnMoves(moveList, pieces[wp], white, wp); break;
				case wr: rookMoves(moveList, pieces[wr], white, wr); break;
				case wn: knightMoves(moveList, pieces[wn], white, wn); break;
				case wb: queen_and_bishopMoves(moveList, pieces[wb], BISHOP_ATTACKS, white, wb); break;
				case wq: queen_and_bishopMoves(moveList, pieces[wq], QUEEN_ATTACKS,  white, wq); break;
				case wk: kingMoves(moveList, pieces[wk], white, wk); break;
				}
			}
		}
		 //  Castling permission King-rook path is not obstructed and not under attack
		if (castlingRights & castle_K && !(allPos & 0x6ull) && !(blackAtt & 0xEull)) { // White King can castle
			moveList.push_back(Move(castlingRights, WCASTLE));
		}
		if (castlingRights & castle_Q && !(allPos & 0x70ull) && !(blackAtt & 0x38ull)) { // White King can castle queenside
			moveList.push_back(Move(castlingRights, WCASTLE_2));
		}
	}
	// If opponent rook has been captured, he looses castling rights.
	// TODO: Needs nicer solution
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

void Board::makeMove(const Move& move, color side)
{
	switch (move_type(move.flags)){
		case MOVE:
			// Piece disappears from from-square and appears at to-square:
			pieces[move.pieces] ^= (bit_at(move.from) | bit_at(move.to));
			// Update Hashkey
			hashKey ^= randomSet[move.pieces][move.from] ^ randomSet[move.pieces][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
						  : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case CAPTURE:
			pieces[move_piece(move.pieces)] ^= (bit_at(move.from) | bit_at(move.to));
			pieces[target_piece(move.pieces)] ^= bit_at(move.to);    // Captured piece is deleted
			hashKey ^= randomSet[move_piece(move.pieces)][move.from] // Update hashKey...
			         ^ randomSet[move_piece(move.pieces)][move.to]
			         ^ randomSet[target_piece(move.pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ bit_at(move.from)) | bit_at(move.to);
				whitePos ^= bit_at(move.to);
			}
			else {
				whitePos = (whitePos ^ bit_at(move.from)) | bit_at(move.to);
				blackPos ^= bit_at(move.to);
			}
			break;
		case PAWN2:
			pieces[move.pieces] ^= (bit_at(move.from) | bit_at(move.to));     // Piece disappears from departure
			hashKey ^= randomSet[move.pieces][move.from] // Update hashKey...
			         ^ randomSet[move.pieces][move.to]
			         ^ randomSet[ENPASSENT_HASH][move.from % 8];

			// The other player can then sometimes perform enpassent (if other pawn is available)
			if (move.pieces == bp && (0x5ull << (move.to - 1)) & (_row << 32) & pieces[wp]) {
				w_enpassent = (move.from % 8) + 1;
				b_enpassent = 0; // Maybe unnecessary
			}
			else if (move.pieces == wp && (0x5ull << (move.to - 1)) & (_row << 24) & pieces[bp]) {
				b_enpassent = (move.from % 8) + 1;
				w_enpassent = 0;
			}

			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
				          : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case PROMOTION:
			pieces[move_piece(move.pieces)] ^= bit_at(move.from);    // removes pawn
			pieces[target_piece(move.pieces)] |= bit_at(move.to);    // New piece appears
			hashKey ^= randomSet[move_piece(move.pieces)][move.from] // Update hashKey...
			         ^ randomSet[target_piece(move.pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
				          : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case C_PROMOTION:
			// QN | Captured
			if (side == black) {
				pieces[bp] ^= bit_at(move.from);
				pieces[move_piece(move.pieces)] ^= bit_at(move.to);
				pieces[target_piece(move.pieces)] |= bit_at(move.to);
				hashKey ^= randomSet[bp][move.from]
				         ^ randomSet[move_piece(move.pieces)][move.to]
				         ^ randomSet[target_piece(move.pieces)][move.to];

				blackPos = (blackPos ^ bit_at(move.from)) | bit_at(move.to);
				whitePos ^= bit_at(move.to);
			}
			else {
				pieces[wp] ^= bit_at(move.from);
				pieces[move_piece(move.pieces)] ^= bit_at(move.to);
				pieces[target_piece(move.pieces)] |= bit_at(move.to);
				hashKey ^= randomSet[wp][move.from]
				        ^ randomSet[move_piece(move.pieces)][move.to]
				        ^ randomSet[target_piece(move.pieces)][move.to];

				whitePos = (whitePos ^ bit_at(move.from)) | bit_at(move.to);
				blackPos ^= bit_at(move.to);
			}
			break;
		case BCASTLE: // Kingside castling
			castlingRights &= ~(castle_k | castle_q);
			pieces[bk]  = bit_at(g8);
			pieces[br] ^= (bit_at(h8) | bit_at(f8));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_k];
			// Override position
			blackPos ^= blackPos & 0xF00000000000000ull;
			blackPos |= 0x600000000000000ull;
			break;
		case WCASTLE: // Castling short
			castlingRights &= ~(castle_K | castle_Q);
			pieces[wk]  = bit_at(g1);
			pieces[wr] ^= (bit_at(h1) | bit_at(f1));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_K];
			// Override position
			whitePos ^= whitePos & 0xF;
			whitePos |= 0x6;
			break;
		case BCASTLE_2: // Castling long
			castlingRights &= ~(castle_k | castle_q);
			pieces[bk]  = bit_at(c8);
			pieces[br] ^= (bit_at(a8) | bit_at(d8));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				    ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_q];
			// Override position
			blackPos ^= blackPos & 0xF800000000000000ull;
			blackPos |= 0x3000000000000000ull;
			break;
		case WCASTLE_2: // Castling long
			castlingRights &= ~(castle_K | castle_Q);
			pieces[wk]  = bit_at(c1);
			pieces[wr] ^= (bit_at(a1) | bit_at(d1));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x30ull;
			break;
		case ENPASSENT:
			if (move.pieces == bp) {
				// Update hashkey
				hashKey ^= randomSet[bp][move.from] 
					     ^ randomSet[bp][move.to] 
					     ^ randomSet[wp][move.to + 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				// Update positions
				pieces[bp] ^= (bit_at(move.from) | bit_at(move.to));
				pieces[wp] ^= bit_at(move.to + 8);

				blackPos = (blackPos ^ bit_at(move.from)) | bit_at(move.to);
				whitePos ^= bit_at(move.to + 8);
				// No more enpassent squares after enpassent
				b_enpassent = 0;
			}
			else {
				hashKey ^= randomSet[wp][move.from]
					     ^ randomSet[wp][move.to]
					     ^ randomSet[bp][move.to - 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				pieces[wp] ^= (bit_at(move.from) | bit_at(move.to));
				pieces[bp] ^= bit_at(move.to - 8);

				whitePos = (whitePos ^ bit_at(move.from)) | bit_at(move.to);
				blackPos ^= bit_at(move.to - 8);
				w_enpassent = 0;
			}
			break;
		default:
			cerr << "Invalid move info encountered!\n";
			exit(1);
	}
	// No enpassent squares after any other move than double pawn push
	if (move_type(move.flags) != PAWN2) {
		b_enpassent = w_enpassent = 0;
	}
	// Check if castling still permitted
	byte cast = move.flags >> 4;
	if (cast){
		castlingRights &= ~cast;
		hashKey ^= randomSet[CASTLE_HASH][castlingRights];
	}
	allPos = blackPos | whitePos;
}

void Board::unMakeMove(const Move& move, color side)
{
	// TODO: UnmakeMove seems to be more performance intensive than makeMove
	switch (move_type(move.flags)){
		case MOVE:
			// Piece disappears from to-square and appears at from-square:
			pieces[move.pieces] ^= (bit_at(move.to) | bit_at(move.from));
			// Update hashKey...
			hashKey ^= randomSet[move.pieces][move.from] ^ randomSet[move.pieces][move.to];
			// Update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
						  : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case CAPTURE:
			pieces[move_piece(move.pieces)]   ^= (bit_at(move.to) | bit_at(move.from));
			pieces[target_piece(move.pieces)] |= bit_at(move.to);
			hashKey ^= randomSet[move_piece(move.pieces)][move.from]
			         ^ randomSet[move_piece(move.pieces)][move.to]
			         ^ randomSet[target_piece(move.pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ bit_at(move.to)) | bit_at(move.from);
				whitePos |= bit_at(move.to);
			}
			else {
				whitePos = (whitePos ^ bit_at(move.to)) | bit_at(move.from);
				blackPos |= bit_at(move.to);
			}
			break;
		case PAWN2:
			pieces[move.pieces] ^= (bit_at(move.to) | bit_at(move.from));     // Piece disappears from destination
			hashKey ^= randomSet[move.pieces][move.to]  // Update hashKey...
			         ^ randomSet[move.pieces][move.from]
			         ^ randomSet[ENPASSENT_HASH][move.from % 8];
			b_enpassent = w_enpassent = 0x0;
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
			              : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case PROMOTION:
			pieces[move_piece(move.pieces)]   |= bit_at(move.from);
			pieces[target_piece(move.pieces)] ^= bit_at(move.to);
			hashKey ^= randomSet[move_piece(move.pieces)][move.from]
			         ^ randomSet[target_piece(move.pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
				          : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case C_PROMOTION:
			if (side == black) {
				pieces[bp] |= bit_at(move.from);
				pieces[move_piece(move.pieces)] |= bit_at(move.to);
				pieces[target_piece(move.pieces)] ^= bit_at(move.to);
				hashKey ^= randomSet[bp][move.from]
				         ^ randomSet[move_piece(move.pieces)][move.to]
				         ^ randomSet[target_piece(move.pieces)][move.to];
			
				blackPos = (blackPos | bit_at(move.from)) ^ bit_at(move.to);
				whitePos |= bit_at(move.to);
			}
			else {
				pieces[wp] |= bit_at(move.from);
				pieces[move_piece(move.pieces)] |= bit_at(move.to);
				pieces[target_piece(move.pieces)] ^= bit_at(move.to);
				hashKey ^= randomSet[wp][move.from]
				         ^ randomSet[move_piece(move.pieces)][move.to]
				         ^ randomSet[target_piece(move.pieces)][move.to];
			
				whitePos = (whitePos | bit_at(move.from)) ^ bit_at(move.to);
				blackPos |= bit_at(move.to);
			}
			break;
		case BCASTLE:
			pieces[bk]  = bit_at(e8);
			pieces[br] ^= (bit_at(f8) | bit_at(h8));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_k];
			// Override position
			blackPos ^= blackPos & 0xF00000000000000ull;
			blackPos |= 0x900000000000000ull;
			castlingRights = move.from;
			break;
		case WCASTLE: // Castling short
			pieces[wk]  = bit_at(e1);
			pieces[wr] ^= (bit_at(f1) | bit_at(h1));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				     ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_K]; 
			// Override position
			whitePos ^= whitePos & 0xF;
			whitePos |= 0x9;
			castlingRights = move.from;
			break;
		case BCASTLE_2:
			pieces[bk]  = bit_at(e8);
			pieces[br] ^= (bit_at(d8) | bit_at(a8));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_q];
			// Override position
			blackPos ^= blackPos & 0xF800000000000000ull;
			blackPos |= 0x8800000000000000ull;
			castlingRights = move.from;
			break;
		case WCASTLE_2: // Castling long
			pieces[wk]  = bit_at(e1);
			pieces[wr] ^= (bit_at(d1) | bit_at(a1));
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x88ull;
			castlingRights = move.from;
			break;
		case ENPASSENT:
			if (move.pieces == bp) {
				// Update hashkey
				hashKey ^= randomSet[bp][move.from] 
					     ^ randomSet[bp][move.to] 
					     ^ randomSet[wp][move.to + 8]
						 ^ randomSet[ENPASSENT_HASH][move.from % 8];
				b_enpassent = (move.to % 8) + 1;
				pieces[bp] ^= (bit_at(move.to) | bit_at(move.from));
				pieces[wp] |= bit_at(move.to + 8);

				blackPos ^= bit_at(move.to);
				blackPos |= bit_at(move.from);
				whitePos |= bit_at(move.to + 8);
			}
			else {
				hashKey ^= randomSet[wp][move.from]
					     ^ randomSet[wp][move.to]
					     ^ randomSet[bp][move.to - 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				w_enpassent = (move.to % 8) + 1;
				pieces[wp] ^= (bit_at(move.to) | bit_at(move.from));
				pieces[bp] |= bit_at(move.to - 8);

				whitePos ^= bit_at(move.to);
				whitePos |= bit_at(move.from);
				blackPos |= bit_at(move.to - 8);
			}
			break;
		default:
			cerr << "Invalid move encountered!\n";
			exit(1);
	}
	// restore some castling rights
	byte cast = (move.flags & 0xF0ull) >> 4;
	if (cast) {
		hashKey ^= randomSet[CASTLE_HASH][castlingRights];
		castlingRights |= cast;
	}
	allPos = blackPos | whitePos;
}

bool Board::isKingInCheck(color kingColor) const
{
	// Returns true if king is in check. Requires accurate attack sets.
	return kingColor == black ? (pieces[bk] & whiteAtt) != 0 : (pieces[wk] & blackAtt) != 0;
}

bool Board::isKingLeftInCheck(color kingColor, const Move& lastMove)
{
	// Returns true if last played move leaves king in check. 
	piece king = kingColor == white ? wk : bk;
	byte kingPos = msb(pieces[king]);
	u64 kingRect = 0x0, kingDiags = 0x0;
	if (move_type(lastMove.flags) > 5) return false;      // Castling does not put king in check
	
	if (KNIGHT_ATTACKS[kingPos] & pieces[kingColor == white ? bn : wn]) return true; // King attacked by opponent knight
	if (KING_ATTACKS[kingPos] & pieces[kingColor == white ? bk : wk]) return true;   // King attacked by opponent king

	if ((0x5ull << ((kingPos - 1) + (kingColor == white ? 8 : -8))) 
		& (_row << 8*((kingPos/8)+(kingColor==white ? +1 : -1))) 
		& pieces[kingColor == white ? bp : wp]) return true; // King attacked by opponent pawns

	// Check if enemy attack was uncovered by lastMove

	kingRect |= floodFill(pieces[king], ~allPos, n);
	kingDiags |= floodFill(pieces[king], ~allPos, ne);
	kingRect |= floodFill(pieces[king], ~allPos, e);
	kingDiags |= floodFill(pieces[king], ~allPos, se);
	kingRect |= floodFill(pieces[king], ~allPos, s);
	kingDiags |= floodFill(pieces[king], ~allPos, sw);

	kingRect |= (allPos ^ (((allPos|_right) - 2 * pieces[king]))) & ~_right;
	kingDiags |= floodFill(pieces[king], ~allPos, nw);


	kingRect  &= ~(kingColor == white ? whitePos : blackPos);
	kingDiags &= ~(kingColor == white ? whitePos : blackPos);

	if (kingColor == white) {
		kingRect  &= (pieces[br] | pieces[bq]);
		kingDiags &= (pieces[bb] | pieces[bq]);
	}
	else {
		kingRect  &= (pieces[wr] | pieces[wq]);
		kingDiags &= (pieces[wb] | pieces[wq]);
	}
	BITLOOP(enemyPos, kingRect) {
		if (!(CONNECTIONS[kingPos][enemyPos] & allPos)) return true;
	}
	BITLOOP(enemyPos, kingDiags) {
		if (!(CONNECTIONS[kingPos][enemyPos] & allPos)) return true;
	}
	// King not under attack
	return false;
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

	// Rewards points, if positions are similar to piece-square-heuristics
	// Pawns:
	int psh = 0;
	u64 mask = pieces[wp];
	BITLOOP(pos, mask)
		psh += pieceSquareTable[0][63 - pos];
	mask = pieces[bp];
	BITLOOP(pos, mask)
		psh -= pieceSquareTable[0][pos];
	mask = pieces[wn];
	BITLOOP(pos, mask)
		psh += pieceSquareTable[1][63 - pos];
	mask = pieces[bn];
	BITLOOP(pos, mask)
		psh -= pieceSquareTable[1][pos];
	mask = pieces[wb];
	BITLOOP(pos, mask)
		psh += pieceSquareTable[2][63 - pos];
	mask = pieces[bb];
	BITLOOP(pos, mask)
		psh -= pieceSquareTable[2][pos];

	if (endGameValue > 0.8) {
		mask = pieces[wk];
		BITLOOP(pos, mask)
			psh += pieceSquareTable[4][63 - pos];
		mask = pieces[bk];
		BITLOOP(pos, mask)
			psh -= pieceSquareTable[4][pos];
	}
	else {
		mask = pieces[wk];
		BITLOOP(pos, mask)
			psh += pieceSquareTable[3][63 - pos];
		mask = pieces[bk];
		BITLOOP(pos, mask)
			psh -= pieceSquareTable[3][pos];
	}
	total_boardValue += psh / 10;

	// *************************** MOBILITY ***************************
	// Determines how many squares are accessible, worth 10 cp each
	int mobility = 0;
	for_white(i) mobility += popcount(attacks[i] ^ blackPos);
	for_black(i) mobility -= popcount(attacks[i] ^ whitePos);
	total_boardValue += mobility * 10;
	mobility = 0;
	// Measure "hostiliy" = number of attacked pieces of opponent. 15cp each
	for_white(i) mobility += popcount(attacks[i] & blackPos);
	for_black(i) mobility += popcount(attacks[i] & whitePos);
	total_boardValue += mobility * 15;
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
		for (int i = 0; i < 8; ++i) mask |= floodFill(mask, ~allPos, (dir)i);
		total_boardValue += 10 * popcount(mask & ~blackAtt);
		mask = pieces[bk];
		for (int i = 0; i < 8; ++i) mask |= floodFill(mask, ~allPos, (dir)i);
		total_boardValue -= 10 * popcount(mask & ~whiteAtt);
	}

	// Pawn shield. Count number of pawns in front of kings in a 2x3 area 0x707
	// Kings on the edges are not being rewarded points

	total_boardValue += popcount((0x707ull >> (msb(pieces[wk] & _noSides) - 3)) & pieces[wp]);
	total_boardValue -= popcount((0x707ull << (msb(pieces[bk] & _noSides) + 3)) & pieces[bp]);

	return (side == white ? 1 : -1) * total_boardValue;
}

unsigned inline Board::blockedPawn(color col)
{
	// Returns number of blocked pawns.
	// Pawns can be blocked by pieces of any color
	if (col == black)
		 return (unsigned) popcount((pieces[bp] >> 8) & allPos);
	else return (unsigned) popcount((pieces[wp] << 8) & allPos);
}

void Board::print() const
{
	// Print full chessboard with symbols and borders without attacked squares
	vector<string> asciiBoard = vector<string>(8, string(8, '.'));
	for (int p = 0; p < 12; p++) {
		auto temp = pieces[p];
		auto count = -1;
		for (u64 b = _msb; b != 0; b >>= 1) {
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