#include "Board.h"

// TODO: King cannot walk in front of pawn, (solved, needs testing)

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
	blackPos = whitePos = castlingRights 
		= b_enpassent = w_enpassent = bpMove = wpMove = 0x0;

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
		if (fenArgs.size() != 5) {
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
		for (int p = 0; p < 6; p++) blackPos |= pieces[p];
		for (int p = 6; p < 12; p++) whitePos |= pieces[p];
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
	int count = 0;
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
	for (int i = 0; i < 12; i++){
		 // Generate only if bitboard not empty
		if(pieces[i])
			updateAttack((piece)i);
		else
			attacks[i] = 0x0;
	}
	// Exclude pieces that attack pieces of same color
	BLACKLOOP(i) attacks[i] &= ~blackPos;
	WHITELOOP(i) attacks[i] &= ~whitePos;
	BLACKLOOP(i) blackAtt   |= attacks[i];
	WHITELOOP(i) whiteAtt   |= attacks[i];
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

u64 Board::floodFill(u64 propagator, u64 empty, dir direction)
{
	// Calculates all attacks including attacked pieces for sliding pieces
	// (Queen, Rook, bishop)(s)
	u64 flood = propagator;
	empty &= noWrap[direction];
	auto r_shift = shift[direction];
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= rotate_l64(propagator, r_shift) & empty;
	return rotate_l64(flood, r_shift) & noWrap[direction];
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
// TODO: Corrected Pawn attacks. Needs testing
void Board::generateMoveList(MoveList& moveList, color side) const
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
	if (side == black){ ////////////////////////////////////////////////BLACK MOVE GENERATION///////////////////////////////////////////////////
		BLACKLOOP(b) { // Loop through black pieces
			attackingPieces = pieces[b];
			if (attackingPieces) { // Only consider non-empty boards
				switch (b) {
				case bp:
					// Find normal captures:
					BITLOOP(pos, attackingPieces) {
						attackMask = ((bit_at(pos) >> 9) & ~_left)  & whitePos;
						attackMask |= ((bit_at(pos) >> 7) & ~_right) & whitePos;
						if (attackMask)                                              // If pieces are targeted
							WHITELOOP(candidate) {                                    // Find targeted piece
							pieceAttacks = pieces[candidate] & attackMask;           // Set specific attacks
							if (pieceAttacks) {
								BITLOOP(target, pieceAttacks) {                       // Add moves
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
							moveList.push_back(Move(24+b_enpassent, 15 + b_enpassent, ENPASSENT, bp));
						}
						if (bit_at(22 + b_enpassent) & pieces[bp] & (_row << 24)) {
							// black pawn left of ep square
							moveList.push_back(Move(22 + b_enpassent, 15 + b_enpassent, ENPASSENT, bp));
						}
					}
					break;
				case br: // BLACK ROOK
					// Calculate attacked pieces
					BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind br
						attackMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br] & whitePos; // Intersections with opponent pieces
						if (attackMask) {                                                // If pieces are targeted
							WHITELOOP(candidate) {                                        // Find targeted piece
								pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
								if (pieceAttacks) {
									BITLOOP(target, pieceAttacks) {                                    // Add moves
										if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
											moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & (pos == h8 ? castle_k : castle_q)), piece_pair(br, candidate)));
										}
									}
								}
							}
						}
						attackMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br]; // Non capturing moves
						BITLOOP(target, attackMask) {                                             // Add moves
							if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
								if (pos == a8) {
									moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & castle_q), br));
								}
								else if (pos == h8) {
									moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & castle_k), br));
								}
								else {
									moveList.push_back(Move(pos, target, MOVE, br));
								}
							}
						}
					}
					break;
				case bn: //// BLACK KNIGHT

					// Calculate attacked pieces
					BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bn
						attackMask = KNIGHT_ATTACKS[pos] & attacks[bn] & whitePos; // Intersections with opponent pieces
						if (attackMask) {                                                // If pieces are targeted
							WHITELOOP(candidate) {                                    // Find targeted piece
								pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
								if (pieceAttacks) {
									BITLOOP(target, pieceAttacks) {                                    // Add moves
										moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(bn, candidate)));
									}
								}
							}
						}
						attackMask ^= KNIGHT_ATTACKS[pos] & attacks[bn]; // Non capturing moves
						BITLOOP(target, attackMask) {                        // Add moves
							moveList.push_back(Move(pos, target, MOVE, bn));
						}
					}
					break;
				case bb: // BLACK BISHOP
					BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bb
						attackMask = BISHOP_ATTACKS[pos] & attacks[bb] & whitePos; // Intersections with opponent pieces
						if (attackMask) {                                                // If pieces are targeted
							WHITELOOP(candidate) {                                    // Find targeted piece
								pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
								if (pieceAttacks) {
									BITLOOP(target, pieceAttacks) {                                    // Add moves
										if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
											moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(bb, candidate)));
										}
									}
								}
							}
						}
						attackMask ^= BISHOP_ATTACKS[pos] & attacks[bb]; // Non capturing moves
						BITLOOP(target, attackMask) {                        // Add moves
							if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
								moveList.push_back(Move(pos, target, MOVE, bb));
							}
						}
					}
					break;
				case bq: // BLACK QUEEN
					BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bq
						attackMask = QUEEN_ATTACKS[pos] & attacks[bq] & whitePos; // Intersections with opponent pieces
						if (attackMask) {                                                // If pieces are targeted
							WHITELOOP(candidate) {                                    // Find targeted piece
								pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
								if (pieceAttacks) {
									BITLOOP(target, pieceAttacks) {                                    // Add moves
										if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
											moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(bq, candidate)));
										}
									}
								}
							}
						}
						attackMask ^= QUEEN_ATTACKS[pos] & attacks[bq]; // Non capturing moves
						BITLOOP(target, attackMask) {                        // Add moves
							if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
								moveList.push_back(Move(pos, target, MOVE, bq));
							}
						}
					}
					break;
				case bk: // BLACK KING
					// Calculate attacked pieces
					BITLOOP(pos, attackingPieces) {                             // Loop through all positions of pieces of kind bk
						attackMask = ((KING_ATTACKS[pos] & attacks[bk] & whitePos) & ~whiteAtt); // Intersections with opponent pieces that would not place king in check
						if (attackMask) {                                                // If pieces are targeted
							WHITELOOP(candidate) {                                    // Find targeted piece
								pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
								if (pieceAttacks) {
									BITLOOP(target, pieceAttacks) {                                 // Add moves
										moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & (castle_k | castle_q)), piece_pair(bk, candidate)));
									}
								}
							}
						}
						attackMask ^= (KING_ATTACKS[pos] & attacks[bk]) & ~whiteAtt; // Non capturing moves
						BITLOOP(target, attackMask) {                        // Add moves
							moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & (castle_k | castle_q)), bk));
						}
					}
					break;
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
 else{////////////////////////////////////////////////WHITE MOVE GENERATION///////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 WHITELOOP(w) { // Loop through white pieces
		 attackingPieces = pieces[w];
		 if (attackingPieces){
			 switch (w) {
			 case wp:
				 // Find normal captures:
				 // attackingPieces stands for attacked squares in this case
				 BITLOOP(pos, attackingPieces) {
					 attackMask = (bit_at(pos) << 9 & ~_right) & blackPos;
					 attackMask |= (bit_at(pos) << 7 & ~_left)  & blackPos;

					 if (attackMask) {                                              // If pieces are targeted
						 BLACKLOOP(candidate) {                                    // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;           // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                       // Add moves
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
				 break;
			 case wr: // WHITE ROOK
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces) {                                                          // Loop through all positions of pieces of kind br
					 attackMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr] & blackPos; // Intersections with opponent pieces
					// printBitboard(attackMask);
					 if (attackMask) {                                                    // If pieces are targeted
						 BLACKLOOP(candidate) {                                        // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;                        // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                                        // Add moves..
									 if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
										 moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & (pos == h1 ? castle_K : castle_Q)), piece_pair(wr, candidate)));
									 }
								 }
							 }
						 }
					 }
					 attackMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr]; // Non capturing moves
					// printBitboard(attackMask);
					 //printBitboard(tempMask);
					 BITLOOP(target, attackMask) {                        // Add moves..
						 if (!(CONNECTIONS[pos][target] & allPos)) {       // ..if no piece is in the way
							 if (pos == a1) {
								 moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & castle_Q), wr));
							 }
							 else if (pos == h1) {
								 moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & castle_K), wr));
							 }
							 else {
								 moveList.push_back(Move(pos, target, MOVE, wr));
							 }
						 }
					 }
				 }
				 break;
			 case wn: //// WHITE KNIGHT
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bn
					 attackMask = KNIGHT_ATTACKS[pos] & attacks[wn] & blackPos;     // Intersections with opponent pieces
					 if (attackMask) {                                                // If pieces are targeted
						 BLACKLOOP(candidate) {                                    // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                                    // Add moves
									 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(wn, candidate)));
								 }
							 }
						 }
					 }
					 attackMask ^= KNIGHT_ATTACKS[pos] & attacks[wn]; // Non capturing moves
					 BITLOOP(target, attackMask) {                        // Add moves
						 moveList.push_back(Move(pos, target, MOVE, wn));
					 }
				 }
				 break;
			 case wb: // WHITE BISHOP
				 BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bb
					 attackMask = BISHOP_ATTACKS[pos] & attacks[wb] & blackPos;     // Intersections with opponent pieces
					 if (attackMask) {                                                // If pieces are targeted
						 BLACKLOOP(candidate) {                                    // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                                    // Add moves
									 if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
										 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(wb, candidate)));
									 }
								 }
							 }
						 }
					 }
					 attackMask ^= BISHOP_ATTACKS[pos] & attacks[wb]; // Non capturing moves
					 BITLOOP(target, attackMask) {                        // Add moves
						 if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
							 moveList.push_back(Move(pos, target, MOVE, wb));
						 }
					 }
				 }
				 break;
			 case wq: // WHITE QUEEN
				 BITLOOP(pos, attackingPieces) {                                   // Loop through all positions of pieces of kind bq
					 attackMask = QUEEN_ATTACKS[pos] & attacks[wq] & blackPos; // Intersections with opponent pieces
					 if (attackMask) {                                                // If pieces are targeted
						 BLACKLOOP(candidate) {                                    // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                                    // Add moves
									 if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
										 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, piece_pair(wq, candidate)));
									 }
								 }
							 }
						 }
					 }
					 attackMask ^= QUEEN_ATTACKS[pos] & attacks[wq]; // Non capturing moves
					 BITLOOP(target, attackMask) {                        // Add moves
						 if (!(CONNECTIONS[pos][target] & allPos)) {   // ..if no piece is in the way
							 moveList.push_back(Move(pos, target, MOVE, wq));
						 }
					 }
				 }
				 break;
			 case wk: // WHITE KING
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces) {                                               // Loop through all positions of pieces of kind wk
					 attackMask = ((KING_ATTACKS[pos] & attacks[wk] & blackPos) & ~blackAtt); // Captures of opponent pieces that would not place king in check
					 if (attackMask) {                                                          // If pieces are targeted
						 BLACKLOOP(candidate) {                                                // Find targeted piece
							 pieceAttacks = pieces[candidate] & attackMask;                       // Set specific attacks
							 if (pieceAttacks) {
								 BITLOOP(target, pieceAttacks) {                                    // Add moves
									 moveList.insert(moveList.begin(), Move(pos, target, move_metadata(CAPTURE, castlingRights & (castle_K | castle_Q)), piece_pair(wk, candidate)));
								 }
							 }
						 }
					 }
					 attackMask ^= (KING_ATTACKS[pos] & attacks[wk]) & ~blackAtt; // Non capturing moves
					 BITLOOP(target, attackMask) {                         // Add moves
						 moveList.push_back(Move(pos, target, move_metadata(MOVE, castlingRights & (castle_K | castle_Q)), wk));
					 }
				 }
				 break;
			}
		}
	 }
	 if (castlingRights & castle_K && !(allPos & 0x6ull) && !(blackAtt & 0xEull)) { // White King can castle
		 moveList.push_back(Move(castlingRights, WCASTLE));
	 }
	 if (castlingRights & castle_Q && !(allPos & 0x70ull) && !(blackAtt & 0x38ull)) { // White King can castle (big)
		 moveList.push_back(Move(castlingRights, WCASTLE_2));
	 }
	}
	// If opponent rook has been captured, he looses castling rights.
	// TODO: Needs nicer solution
	for_each(moveList.begin(), moveList.end(), [this](Move& move) {
		if (target_piece(move.Pieces) == wr) {
			if ((move.flags & 0xF) == CAPTURE || (move.flags & 0xF) == C_PROMOTION) {
				if (move.to == a1) 
					move.flags |= (castlingRights & castle_Q) << 4; 
				else if (move.to == h1) 
					move.flags |= (castlingRights & castle_K) << 4;
			}
		}
		else if (target_piece(move.Pieces) == br) {
			if ((move.flags & 0xF) == CAPTURE || (move.flags & 0xF) == C_PROMOTION) {
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
	switch (move.flags & 0xFull){
		case MOVE:
			pieces[move.Pieces] ^= bit_at(move.from);     // Piece disappears from departure
			pieces[move.Pieces] |= bit_at(move.to);       // Piece appears at destination
			hashKey ^= randomSet[move.Pieces][move.from]  // Update hashKey...
			         ^ randomSet[move.Pieces][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
						  : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case CAPTURE:
			pieces[move_piece(move.Pieces)] ^= bit_at(move.from);     // Piece disappears from departure
			pieces[move_piece(move.Pieces)] |= bit_at(move.to);       // Piece appears at destination
			pieces[target_piece(move.Pieces)] ^= bit_at(move.to);    // Captured piece is deleted
			hashKey ^= randomSet[move_piece(move.Pieces)][move.from] // Update hashKey...
			         ^ randomSet[move_piece(move.Pieces)][move.to]
			         ^ randomSet[target_piece(move.Pieces)][move.to];
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
			pieces[move.Pieces] ^= bit_at(move.from);     // Piece disappears from departure
			pieces[move.Pieces] |= bit_at(move.to);       // Piece appears at destination
			hashKey ^= randomSet[move.Pieces][move.from] // Update hashKey...
			         ^ randomSet[move.Pieces][move.to]
			         ^ randomSet[ENPASSENT_HASH][move.from % 8];

			// The other player can then sometimes perform enpassent (if other pawn is available)
			if (move.Pieces == bp && (0x5ull << (move.to - 1)) & (_row << 32) & pieces[wp]) {
				w_enpassent = (move.from % 8) + 1;
			}
			else if (move.Pieces == wp && (0x5ull << (move.to - 1)) & (_row << 24) & pieces[bp]) {
				b_enpassent = (move.from % 8) + 1;
			}

			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
				          : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case PROMOTION:
			pieces[move_piece(move.Pieces)] ^= bit_at(move.from);    // removes pawn
			pieces[target_piece(move.Pieces)] |= bit_at(move.to);    // New piece appears
			hashKey ^= randomSet[move_piece(move.Pieces)][move.from] // Update hashKey...
			         ^ randomSet[target_piece(move.Pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
				          : (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
			break;
		case C_PROMOTION:
			// QN | Captured
			if (side == black) {
				pieces[bp] ^= bit_at(move.from);
				pieces[move_piece(move.Pieces)] ^= bit_at(move.to);
				pieces[target_piece(move.Pieces)] |= bit_at(move.to);
				hashKey ^= randomSet[bp][move.from]
				         ^ randomSet[move_piece(move.Pieces)][move.to]
				         ^ randomSet[target_piece(move.Pieces)][move.to];

				blackPos = (blackPos ^ bit_at(move.from)) | bit_at(move.to);
				whitePos ^= bit_at(move.to);
			}
			else {
				pieces[wp] ^= bit_at(move.from);
				pieces[move_piece(move.Pieces)] ^= bit_at(move.to);
				pieces[target_piece(move.Pieces)] |= bit_at(move.to);
				hashKey ^= randomSet[wp][move.from]
				        ^ randomSet[move_piece(move.Pieces)][move.to]
				        ^ randomSet[target_piece(move.Pieces)][move.to];

				whitePos = (whitePos ^ bit_at(move.from)) | bit_at(move.to);
				blackPos ^= bit_at(move.to);
			}
			break;
		case BCASTLE: // Kingside castling
			castlingRights &= ~(castle_k | castle_q);
			pieces[bk]  = bit_at(g8);
			pieces[br] ^= bit_at(h8);
			pieces[br] |= bit_at(f8);
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
			pieces[wr] ^= bit_at(h1);
			pieces[wr] |= bit_at(f1);
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
			pieces[br] ^= bit_at(a8);
			pieces[br] |= bit_at(d8);
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
			pieces[wr] ^= bit_at(a1);
			pieces[wr] |= bit_at(d1);
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x30ull;
			break;
		case ENPASSENT:
			if (move.Pieces == bp) {
				hashKey ^= randomSet[bp][move.from] 
					     ^ randomSet[bp][move.to] 
					     ^ randomSet[wp][move.to + 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				pieces[bp] ^= bit_at(move.from);
				pieces[bp] |= bit_at(move.to);
				pieces[wp] ^= bit_at(move.to + 8);
				b_enpassent = 0;
			}
			else {
				hashKey ^= randomSet[wp][move.from]
					     ^ randomSet[wp][move.to]
					     ^ randomSet[bp][move.to - 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				pieces[wp] ^= bit_at(move.from);
				pieces[wp] |= bit_at(move.to);
				pieces[bp] ^= bit_at(move.to - 8);
				w_enpassent = 0;
			}
			break;
		default:
			cerr << "Invalid move info encountered!\n";
			exit(1);
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
	switch (move.flags & 0xFull){
		case MOVE:
			pieces[move.Pieces] ^= bit_at(move.to);       // Piece disappears from destination
			pieces[move.Pieces] |= bit_at(move.from);     // Piece reappears at departure
			hashKey ^= randomSet[move.Pieces][move.from]  // Update hashKey...
			        ^ randomSet[move.Pieces][move.to];
			// Update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
						  : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case CAPTURE:
			pieces[move_piece(move.Pieces)]   ^= bit_at(move.to);    // Piece disappears from destination
			pieces[move_piece(move.Pieces)]   |= bit_at(move.from);  // Piece appears at departure
			pieces[target_piece(move.Pieces)] |= bit_at(move.to);    // Captured piece reappears
			hashKey ^= randomSet[move_piece(move.Pieces)][move.from] // Update hashKey...
			         ^ randomSet[move_piece(move.Pieces)][move.to]
			         ^ randomSet[target_piece(move.Pieces)][move.to];
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
			pieces[move.Pieces] ^= bit_at(move.to);     // Piece disappears from destination
			pieces[move.Pieces] |= bit_at(move.from);   // Piece appears at departure
			hashKey ^= randomSet[move.Pieces][move.to]  // Update hashKey...
			         ^ randomSet[move.Pieces][move.from]
			         ^ randomSet[ENPASSENT_HASH][move.from % 8];
			b_enpassent = w_enpassent = 0x0;
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
			              : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case PROMOTION:
			pieces[move_piece(move.Pieces)]    |= bit_at(move.from);
			pieces[target_piece(move.Pieces)] ^= bit_at(move.to);
			hashKey ^= randomSet[move_piece(move.Pieces)][move.from]
			         ^ randomSet[target_piece(move.Pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.to)) | bit_at(move.from)))
				          : (whitePos = ((whitePos ^ bit_at(move.to)) | bit_at(move.from)));
			break;
		case C_PROMOTION:
			if (side == black) {
				pieces[bp] |= bit_at(move.from);
				pieces[move_piece(move.Pieces)] |= bit_at(move.to);
				pieces[target_piece(move.Pieces)] ^= bit_at(move.to);
				hashKey ^= randomSet[bp][move.from]
				         ^ randomSet[move_piece(move.Pieces)][move.to]
				         ^ randomSet[target_piece(move.Pieces)][move.to];
			
				blackPos = (blackPos | bit_at(move.from)) ^ bit_at(move.to);
				whitePos |= bit_at(move.to);
			}
			else {
				pieces[wp] |= bit_at(move.from);
				pieces[move_piece(move.Pieces)] |= bit_at(move.to);
				pieces[target_piece(move.Pieces)] ^= bit_at(move.to);
				hashKey ^= randomSet[wp][move.from]
				         ^ randomSet[move_piece(move.Pieces)][move.to]
				         ^ randomSet[target_piece(move.Pieces)][move.to];
			
				whitePos = (whitePos | bit_at(move.from)) ^ bit_at(move.to);
				blackPos |= bit_at(move.to);
			}
			break;
		case BCASTLE:
			pieces[bk]  = bit_at(e8);
			pieces[br] ^= bit_at(f8);
			pieces[br] |= bit_at(h8);
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
			pieces[wr] ^= bit_at(f1);
			pieces[wr] |= bit_at(h1);
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
			pieces[br] ^= bit_at(d8);
			pieces[br] |= bit_at(a8);
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
			pieces[wr] ^= bit_at(d1);
			pieces[wr] |= bit_at(a1);
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
			         ^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x88ull;
			castlingRights = move.from;
			break;
		case ENPASSENT:
			if (move.Pieces == bp) {
				hashKey ^= randomSet[bp][move.from] 
					     ^ randomSet[bp][move.to] 
					     ^ randomSet[wp][move.to + 8]
						 ^ randomSet[ENPASSENT_HASH][move.from % 8];
				b_enpassent = (move.to % 8) + 1;
				pieces[bp] ^= bit_at(move.to);
				pieces[bp] |= bit_at(move.from);
				pieces[wp] |= bit_at(move.to + 8);
			}
			else {
				hashKey ^= randomSet[wp][move.from]
					     ^ randomSet[wp][move.to]
					     ^ randomSet[bp][move.to - 8]
					     ^ randomSet[ENPASSENT_HASH][move.from % 8];
				w_enpassent = (move.to % 8) + 1;
				pieces[wp] ^= bit_at(move.to);
				pieces[wp] |= bit_at(move.from);
				pieces[bp] |= bit_at(move.to - 8);
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
		auto repChar = [](int c){for(int i = 0; i < c; i++)cout << "\u2588";};
 		repChar(10);cout <<  '\n';
		for (auto r : asciiBoard) {
			cout << "\u2588";
			for (auto c : r) cout << c;
			cout << "\u2588" << '\n';
		}
		repChar(10); cout << "\n\n";
	#endif
}

bool Board::isKingInCheck(color kingColor) const
{
	return kingColor == black ? pieces[bk] & whiteAtt : pieces[wk] & blackAtt;
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
	WHITELOOP(i) mobility += popcount(attacks[i] ^ blackPos);
	BLACKLOOP(i) mobility -= popcount(attacks[i] ^ whitePos);
	total_boardValue += mobility * 10;
	mobility = 0;
	// Measure "hostiliy" = number of attacked pieces of opponent. 15cp each
	WHITELOOP(i) mobility += popcount(attacks[i] & blackPos);
	BLACKLOOP(i) mobility += popcount(attacks[i] & whitePos);
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