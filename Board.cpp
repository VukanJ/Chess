#include "Board.h"

Board::Board()
	: whitePos(0x0), blackPos(0x0), whiteAtt(0x0), blackAtt(0x0), hashKey(0x0),
	castlingRights(0x0), b_enpassent(0x0), w_enpassent(0x0)
{
	pieces = vector<u64>(12, 0x0);
	attacks = vector<u64>(12, 0x0);
	randomSet = hash.getRandomSet();
	sideToMove = black; // To be implemented
}

Board::Board(string fen) : Board()
{
	if (fen == "*"){ // Standard starting position
		for (int i = 0; i < 12; i++) pieces[i] = standardPosition[i];
		blackPos = 0xFFFFull;
		whitePos = 0xFFFF000000000000ull;
		castlingRights = 0xFF;
	}
	else { // Setup board according to FEN
		int counter = -1;
		for (auto& p : fen) {
			if (isdigit(p))
				counter += p - 48;
			else {
				counter++;
				if (p == '/') counter--;
				else pieces[getPieceIndex(p)] |= BIT_AT_R(counter);
			}
		}
		for (int p = 0; p < 6;  p++) blackPos |= pieces[p];
		for (int p = 6; p < 12; p++) whitePos |= pieces[p];
		if (pieces[br] & 0x1 && pieces[bk] & 0x10) castlingRights |= Ck;
		if (pieces[br] & 0x80 && pieces[bk] & 0x10) castlingRights |= CCk;
		if (pieces[wr] & 0x0100000000000000ull && pieces[wk] & 0x1000000000000000ull) castlingRights |= CK;
		if (pieces[wr] & 0x8000000000000000ull && pieces[wk] & 0x1000000000000000ull) castlingRights |= CCK;
	}
	cout << "Castling rights -> ";
	printBits(castlingRights);

	sideToMove = black; // To be implemented
	updateAllAttacks();
	initHash();

	// debug
	printf("blocked black pawns: %d\n", blockedPawn(black));
	printf("blocked white pawns: %d\n", blockedPawn(white));
	auto startingHash = hashKey;
	list<Move> movelist;
	cout << "Board value: " << evaluate() << endl;
	movelist.clear();
	print();
	generateMoveList(movelist, white);


	for (auto& m : movelist){
		cout << moveString(m) << endl;
		makeMove(m);
		print();
		unMakeMove(m);
		print();
	}

	if (isCheckMate(black))
		cout << "CHECKMATE FOR BLACK!\n";
	else if (isCheckMate(white))
		cout << "CHECKMATE FOR WHITE!\n";

	if (startingHash != hashKey){
		cerr << "\t\t\t::: HASHING ERROR :::\n";
		printBitboard(hashKey);
	}
	else clog << "::: HASH OK :::\n";
	if (!pieces[bk] || !pieces[wk])
		cerr << "Missing Kings!!\n";
	if (POPCOUNT(pieces[bk]) > 1 || POPCOUNT(pieces[wk]) > 1){
		// No need to handle multiple kings
		cerr << "Too many kings. Invalid Board!\n" << endl;
		exit(1);
	}
}

void Board::updateAllAttacks()
{
	whiteAtt = blackAtt = 0x0;
	for (int i = 0; i < 12; i++)
		updateAttack((piece)i);
	// Exclude pieces that attack pieces of same color
	BLACKLOOP(i) attacks[i] = attacks[i] & ~blackPos;
	WHITELOOP(i) attacks[i] = attacks[i] & ~whitePos;
	BLACKLOOP(i) blackAtt  |= attacks[i];
	WHITELOOP(i) whiteAtt  |= attacks[i];
}

void Board::updateAttack(piece p)
{
	// Fill all the bits that are attacked by individual pieces
	// including attacked enemy pieces
	// Patterns:
	unsigned long pos = -1;
	u64 mask = 0;
	switch (p){
		case bp:
			pawnFill(black);
			break;
		case br:
			attacks[br] = 0x0;
			for (int i = 0; i < 4;)
				attacks[br] |= floodFill(pieces[br], ~(whitePos | blackPos), (dir)i++);
			break;
		case bn:
			mask = pieces[bn];
			attacks[bn] = 0x0;
			BITLOOP(pos,mask) attacks[bn] |= KNIGHT_ATTACKS[pos] & ~blackPos;
			break;
		case bb:
			attacks[bb] = 0x0;
			for (int i = 4; i < 8;)
				attacks[bb] |= floodFill(pieces[bb], ~(whitePos | blackPos), (dir)i++);
			break;
		case bk:
			mask = pieces[bk];
			attacks[bk] = 0x0;
			BITLOOP(pos, mask) attacks[bk] |= KING_ATTACKS[pos] & ~blackPos;
			break;
		case bq:
			attacks[bq] = 0x0;
			for (int i = 0; i < 8;)
				attacks[bq] |= floodFill(pieces[bq], ~(whitePos | blackPos), (dir)i++);
			break;
		case wp:
			pawnFill(white);
			break;
		case wr:
			attacks[wr] = 0x0;
			for (int i = 0; i < 4;)
				attacks[wr] |= floodFill(pieces[wr], ~(whitePos | blackPos), (dir)i++);
			break;
		case wn:
			mask = pieces[wn];
			attacks[wn] = 0x0;
			BITLOOP(pos, mask) attacks[wn] |= KNIGHT_ATTACKS[pos] & ~whitePos;
			break;
		case wb:
			attacks[wb] = 0x0;
			for (int i = 4; i < 8;)
				attacks[wb] |= floodFill(pieces[wb], ~(whitePos | blackPos), (dir)i++);
			break;
		case wk:
			mask = pieces[wk];
			attacks[wk] = 0x0;
			BITLOOP(pos, mask) attacks[wk] |= KING_ATTACKS[pos] & ~whitePos;
			break;
		case wq:
			attacks[wq] = 0x0;
			for (int i = 0; i < 8;)
				attacks[wq] |= floodFill(pieces[wq], ~(whitePos | blackPos), (dir)i++);
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
	flood |= propagator = ROTL64(propagator, r_shift) & empty;
	flood |= propagator = ROTL64(propagator, r_shift) & empty;
	flood |= propagator = ROTL64(propagator, r_shift) & empty;
	flood |= propagator = ROTL64(propagator, r_shift) & empty;
	flood |= propagator = ROTL64(propagator, r_shift) & empty;
	flood |= ROTL64(propagator, r_shift) & empty;
	return ROTL64(flood, r_shift) & noWrap[direction];
}

void Board::pawnFill(color side)
{
	// Does not compute enPassent
	if (side == black){
		attacks[bp] = 0x0;
		// Normal step
		attacks[bp] |= (pieces[bp] << 8) & ~(whitePos | blackPos);
		// Double step
		attacks[bp] |= ((((pieces[bp] & 0xFF00ull) << 8) & ~(whitePos | blackPos)) << 8) & ~(whitePos | blackPos);
		// Side Attacks
		attacks[bp] |= ((pieces[bp] & _noSides)              << 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & _noSides)              << 7) & whitePos;
		attacks[bp] |= ((pieces[bp] & 0x0101010101010101ull) << 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & 0x8080808080808080ull) << 7) & whitePos;
	}
	else{
		attacks[wp] = 0x0;
		// Normal step
		attacks[wp] |= (pieces[wp] >> 8) & ~(whitePos | blackPos);
		// Double step
		attacks[wp] |= ((((pieces[wp] & 0xFF000000000000ull) >> 8) & ~(whitePos | blackPos)) >> 8) & ~(whitePos | blackPos);
		// Side Attacks
		attacks[wp] |= ((pieces[wp] & _noSides)              >> 9) & blackPos;
		attacks[wp] |= ((pieces[wp] & _noSides)              >> 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & 0x0101010101010101ull) >> 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & 0x8080808080808080ull) >> 9) & blackPos;
	}
}

void Board::initHash()
{
	auto pos = 0, i = 0;
	for (auto p : pieces){
		BITLOOP(pos, p){
			hashKey ^= randomSet[i][pos];
		}
		i++;
	}
	hashKey ^= randomSet[CASTLE_HASH][castlingRights];
	//cout << "Initial hash --> " << hex << hashKey << endl;
}

void Board::generateMoveList(list<Move>& moveList, color side) const
{
	/*
	This method generates a list of all possible moves for a player.
	Aggressive and special moves are generated first and preferably stored
	at the front of the movelist
	*/
	unsigned long pos = nulSq, m = nulSq;
	u64 tempMask = 0x0;
	u64 temp2 = 0x0, attackingPieces = 0x0;
	// Generate all capturing and normal moves
	if (side == black){////////////////////////////////////////////////BLACK MOVE GENERATION///////////////////////////////////////////////////
	BLACKLOOP(b){ // Loop through black pieces
		attackingPieces = pieces[b];
		switch (b){
			case bp:
				// Find normal captures:
				// attackingPieces stands for attacked squares in this case
				BITLOOP(pos, attackingPieces){
					tempMask = (0x5ull << (pos - 1 + 8)) & whitePos;
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2){
							BITLOOP(m, temp2){                                   // Add moves
								if (m > 55){
									moveList.push_front(Move(pos, m, C_PROMOTION, PIECE_PAIR(candidate, bq)));
									moveList.push_front(Move(pos, m, C_PROMOTION, PIECE_PAIR(candidate, bn)));
								}
								else{
									moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(bp, candidate)));
								}
							}
						}
					}
				}
				// Find normal upwards moves and double pawn steps:
				attackingPieces = (pieces[bp] << 8) & ~(blackPos | whitePos);
				BITLOOP(pos, attackingPieces){
					if (pos < 56){
						moveList.push_back(Move(pos - 8, pos, MOVE, bp));
					} else{
						moveList.push_front(Move(pos - 8, pos, PROMOTION, PIECE_PAIR(bp, bq)));
						moveList.push_front(Move(pos - 8, pos, PROMOTION, PIECE_PAIR(bp, bn)));
					}
				}
				attackingPieces = (pieces[bp] << 16) & attacks[bp];
				BITLOOP(pos, attackingPieces)
					moveList.push_back(Move(pos - 16, pos, MOVE, bp));
				break;
			case br: // BLACK ROOK
				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind br
					tempMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br] & whitePos; // Intersections with opponent pieces
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                        // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE | ((pos == 0 ? Ck : CCk) << 4), PIECE_PAIR(br, candidate)));
					}
					tempMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br]; // Non capturing moves
					BITLOOP(m, tempMask)                                             // Add moves
						moveList.push_back(Move(pos, m, MOVE | ((pos == 0 ? Ck : CCk) << 4), br));
				}
				break;
			case bn: //// BLACK KNIGHT

				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bn
					tempMask = KNIGHT_ATTACKS[pos] & attacks[bn] & whitePos; // Intersections with opponent pieces
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(bn,candidate)));
					}
					tempMask ^= KNIGHT_ATTACKS[pos] & attacks[bn]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bn));
				}
				break;
			case bb: // BLACK BISHOP
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bb
					tempMask = BISHOP_ATTACKS[pos] & attacks[bb] & whitePos; // Intersections with opponent pieces
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(bb,candidate)));
					}
					tempMask ^= BISHOP_ATTACKS[pos] & attacks[bb]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bb));
				}
				break;
			case bq: // BLACK QUEEN
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bq
					tempMask = QUEEN_ATTACKS[pos] & attacks[bq] & whitePos; // Intersections with opponent pieces
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(bq,candidate)));
					}
					tempMask ^= QUEEN_ATTACKS[pos] & attacks[bq]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bq));
				}
				break;
			case bk: // BLACK KING

				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                             // Loop through all positions of pieces of kind bk
					tempMask = (KING_ATTACKS[pos] & attacks[bk] & whitePos) & ~whiteAtt; // Intersections with opponent pieces that would not place king in check
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE | ((Ck | CCk) << 4), PIECE_PAIR(bk, candidate)));
					}
					tempMask ^= (KING_ATTACKS[pos] & attacks[bk]) & ~whiteAtt; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE | ((Ck | CCk) << 4), bk));
				}
				break;
			}
		}
		// Generate castling moves
		// Black King can castle if there are no pieces between king and rook, both havent moved yet and king
		// does not cross attacked squares during castling, same for white
		if (castlingRights & Ck && !(( blackPos | whitePos ) & 0x60ull) && !(whiteAtt & 0x70ull))
			moveList.push_back(Move(castlingRights, BCASTLE));
		if (castlingRights & CCk && !(( blackPos | whitePos ) & 0xEull) && !(whiteAtt & 0x1Cull)) // Black King can castle (big)
			moveList.push_back(Move(castlingRights, BCASTLE_2));
	}
 else{////////////////////////////////////////////////WHITE MOVE GENERATION///////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	WHITELOOP(w){ // Loop through black pieces
		 attackingPieces = pieces[w];
		 switch (w){
			 case wp:
				 // Find normal captures:
				 // attackingPieces stands for attacked squares in this case
				 BITLOOP(pos, attackingPieces){
					 tempMask = (0x5ull << (pos - 1 - 8)) & blackPos;
					 if (tempMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						 if (temp2){
							 BITLOOP(m, temp2){                                   // Add moves
								 if (m < 8){
									 moveList.push_front(Move(pos, m, C_PROMOTION, PIECE_PAIR(candidate, wq)|(0x1<<8)));
									 moveList.push_front(Move(pos, m, C_PROMOTION, PIECE_PAIR(candidate, wn)|(0x1<<8)));
								 }
								 else{
									 moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(wp,candidate)));
								 }
							 }
						 }
					 }
				 }
				 // Find normal upwards moves and double pawn steps:
				 attackingPieces = (pieces[wp] >> 8) & ~(blackPos | whitePos);
				 BITLOOP(pos, attackingPieces){
					 if (pos > 7){
						 moveList.push_back(Move(pos + 8, pos, MOVE, wp));
					 }
					 else{
						 moveList.push_front(Move(pos + 8, pos, PROMOTION, PIECE_PAIR(wp, wq)));
						 moveList.push_front(Move(pos + 8, pos, PROMOTION, PIECE_PAIR(wp, wn)));
					 }
				 }
				 attackingPieces = (pieces[wp] >> 16)&attacks[wp];
				 BITLOOP(pos, attackingPieces)
						 moveList.push_back(Move(pos + 16, pos, MOVE, wp));
				 break;
			 case wr: // WHITE ROOK
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                                         // Loop through all positions of pieces of kind br
					 tempMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr] & blackPos; // Intersections with opponent pieces
					 if (tempMask)                                                    // If pieces are targeted
						 BLACKLOOP(candidate){                                        // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                        // Set specific attacks
						 if (temp2)
							 BITLOOP(m, temp2)                                        // Add moves
							 moveList.push_front(Move(pos, m, CAPTURE | ((pos == 56 ? CK : CCK) << 4), PIECE_PAIR(wr, candidate)));
					 }
					 tempMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr]; // Non capturing moves
					 //printBitboard(tempMask);
					 BITLOOP(m, tempMask)                                             // Add moves
						 moveList.push_back(Move(pos, m, MOVE | ((pos == 56 ? CK : CCK) << 4), wr));
				 }
				 break;
			 case wn: //// WHITE KNIGHT

				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bn
					 tempMask = KNIGHT_ATTACKS[pos] & attacks[wn] & blackPos;     // Intersections with opponent pieces
					 if (tempMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						 if (temp2)
							 BITLOOP(m, temp2)                                    // Add moves
							 moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(wn, candidate)));
					 }
					 tempMask ^= KNIGHT_ATTACKS[pos] & attacks[wn]; // Non capturing moves
					 BITLOOP(m, tempMask)                        // Add moves
						 moveList.push_back(Move(pos, m, MOVE, wn));
				 }
				 break;
			 case wb: // WHITE BISHOP
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bb
					 tempMask = BISHOP_ATTACKS[pos] & attacks[wb] & blackPos;     // Intersections with opponent pieces
					 if (tempMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						 if (temp2)
							 BITLOOP(m, temp2)                                    // Add moves
							 moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(wb, candidate)));
					 }
					 tempMask ^= BISHOP_ATTACKS[pos] & attacks[wb]; // Non capturing moves
					 BITLOOP(m, tempMask)                        // Add moves
						 moveList.push_back(Move(pos, m, MOVE, wb));
				 }
				 break;
			 case wq: // WHITE QUEEN
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bq
					 tempMask = QUEEN_ATTACKS[pos] & attacks[wq] & blackPos; // Intersections with opponent pieces
					 if (tempMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						 if (temp2)
							 BITLOOP(m, temp2)                                    // Add moves
							 moveList.push_front(Move(pos, m, CAPTURE, PIECE_PAIR(wq, candidate)));
					 }
					 tempMask ^= QUEEN_ATTACKS[pos] & attacks[wq]; // Non capturing moves
					 BITLOOP(m, tempMask)                        // Add moves
						 moveList.push_back(Move(pos, m, MOVE, wq));
				 }
				 break;
			 case wk: // WHITE KING
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                           // Loop through all positions of pieces of kind wk
					 tempMask = (KING_ATTACKS[pos] & attacks[wk] & blackPos) & ~blackAtt; // Intersections with opponent pieces that would not place king in check
					 if (tempMask)                                                        // If pieces are targeted
						 BLACKLOOP(candidate){                                            // Find targeted piece
						 temp2 = pieces[candidate] & tempMask;                            // Set specific attacks
						 if (temp2)
							 BITLOOP(m, temp2)                                            // Add moves
							 moveList.push_front(Move(pos, m, CAPTURE | ((CK | CCK) << 4), PIECE_PAIR(wk, candidate)));
					 }
					 tempMask ^= (KING_ATTACKS[pos] & attacks[wk]) & ~blackAtt; // Non capturing moves
					 BITLOOP(m, tempMask)                         // Add moves
						 moveList.push_back(Move(pos, m, MOVE|((CK | CCK) << 4), wk));
				 }
				 break;
		 }
	 }
	 // Es muss HIER �berpr�ft werden, ob die Figuren existieren (logisch)
	 if (castlingRights & CK && !((whitePos|blackPos) & 0x6000000000000000ull) && !(blackAtt & 0x7000000000000000ull)) // White King can castle
		moveList.push_back(Move(castlingRights,WCASTLE));
	 if (castlingRights & CCK && !((whitePos | blackPos) & 0xE00000000000000ull) && !(blackAtt & 0x1C00000000000000ull)) // White King can castle (big)
		 moveList.push_back(Move(castlingRights, WCASTLE_2));
 }
}

void Board::makeMove(const Move& move)
{
	switch (move.flags & 0xFull){
		case MOVE:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.from);     // Piece disappears from departure
			pieces[MOV_PIECE(move.Pieces)] |= BIT_AT(move.to);       // Piece appears at destination
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			break;
		case CAPTURE:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.from);     // Piece disappears from departure
			pieces[MOV_PIECE(move.Pieces)] |= BIT_AT(move.to);       // Piece appears at destination
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);    // Captured piece is deleted
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case PROMOTION:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.from);     // removes pawn
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);    // New piece appears
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case C_PROMOTION:
			pieces[((move.Pieces&(0x3 << 8)) >> 8) * 6] ^= BIT_AT(move.from);     // removes pawn
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.to);                    // Captured piece disappears
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);                 // New piece appears
			hashKey ^= randomSet[((move.Pieces&(0x3 << 8)) >> 8) * 6][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case BCASTLE: // Castling short
			castlingRights &= ~(Ck | CCk);                     // No castling rights after castling
			makeMove(Move(d1, b1, MOVE, bk));                  // move king and rook...
			makeMove(Move(a1, c1, MOVE, br));
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]; // update hashKey with new castling rights
			break;
		case WCASTLE: // Castling short
			castlingRights &= ~(CK | CCK);
			makeMove(Move(d8, b8, MOVE, wk));
			makeMove(Move(a8, c8, MOVE, wr));
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			break;
		case BCASTLE_2: // Castling long
			castlingRights &= ~(Ck | CCk);
			makeMove(Move(d1, f1, MOVE, bk));
			makeMove(Move(h1, e1, MOVE, br));
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			break;
		case WCASTLE_2: // Castling long
			castlingRights &= ~(CK | CCK);
			makeMove(Move(d8, f8, MOVE, wk));
			makeMove(Move(h8, e8, MOVE, wr));
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			break;
	}
	// Check if castling still permitted
	byte cast = (move.flags & 0xF0ull)>>4;
	if (cast){
		if (cast & Ck)      { castlingRights &= ~Ck;  }
		else if (cast & CCk){ castlingRights &= ~CCk; }
		if (cast & CK)      { castlingRights &= ~CK;  }
		else if (cast & CCK){ castlingRights &= ~CCK; }
	}
}

void Board::unMakeMove(const Move& move)
{
	switch (move.flags & 0xFull){
		case MOVE:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.to);       // Piece disappears from destination
			pieces[MOV_PIECE(move.Pieces)] |= BIT_AT(move.from);     // Piece reappears at departure
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			break;
		case CAPTURE:
			pieces[MOV_PIECE(move.Pieces)]    ^= BIT_AT(move.to);    // Piece disappears from destination
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.from);  // Piece appears at departure
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);    // Captured piece reappears
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case PROMOTION:
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.from);
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case C_PROMOTION:
			pieces[((move.Pieces&(0x3 << 8)) >> 8) * 6] |= BIT_AT(move.from);     // Pawn reappears
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.to);                 // Captured piece reappears
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);                 // Promoted piece disappears
			hashKey ^= randomSet[((move.Pieces&(0x3 << 8)) >> 8) * 6][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			break;
		case BCASTLE:
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]; // Reapply new castling rights to hash (inverse operation)
			makeMove(Move(b1, d1, MOVE, bk));
			makeMove(Move(c1, a1, MOVE, br));
			castlingRights = move.from;                        // Restore old rights
			break;
		case WCASTLE: // Castling short
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(b8, d8, MOVE, wk));
			makeMove(Move(c8, a8, MOVE, wr));
			castlingRights = move.from;
			break;
		case BCASTLE_2:
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(f1, d1, MOVE, bk));
			makeMove(Move(e1, h1, MOVE, br));
			castlingRights = move.from;
			break;
		case WCASTLE_2: // Castling long
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(f8, d8, MOVE, wk));
			makeMove(Move(e8, h8, MOVE, wr));
			castlingRights = move.from;
			break;
	}
}

bool Board::isCheckMate(color side) const
{
	// Not yet tested
	unsigned long pos;
	u64 mask;
	if (side == black){
		if (whiteAtt & pieces[bk]){
			mask = pieces[bk];
			BITSCANR64(pos, mask);
			return (KING_ATTACKS[pos] & whiteAtt == KING_ATTACKS[pos]) && (pieces[bk] & whiteAtt);
		}
		else return false;
	}
	else{
		if (blackAtt & pieces[bk]){
			mask = pieces[wk];
			BITSCANR64(pos, mask);
			return (KING_ATTACKS[pos] & blackAtt == KING_ATTACKS[pos]) && (pieces[wk] & blackAtt);
		}
		else return false;
	}
	return true;
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
	cout << string(10, '@') << endl;
	for (auto r : asciiBoard) {
		cout << '@';
		for (auto c : r) cout << c;
		cout << "@\n";
	}
	cout << string(10, '@') << '\n';
}

float Board::evaluate()
{
	// Computer is always black

	// Material
	float material = 9.0f * ((float)POPCOUNT(pieces[bq]) - (float)POPCOUNT(pieces[wq]))
		+ 5.0f * ((float)POPCOUNT(pieces[br]) - (float)POPCOUNT(pieces[wr]))
		+ 3.0f * ((float)POPCOUNT(pieces[bb]) - (float)POPCOUNT(pieces[wb]))
		+ 3.0f * ((float)POPCOUNT(pieces[bn]) - (float)POPCOUNT(pieces[wn]))
		+ 2.0f * ((float)POPCOUNT(pieces[bp]) - (float)POPCOUNT(pieces[wp]));
	// Mobility
	float mobility = 0.0f;
	BLACKLOOP(i) mobility += POPCOUNT(attacks[i]);
	WHITELOOP(i) mobility -= POPCOUNT(attacks[i]);
	mobility *= 0.1;
	// WIP: King safety, pawn structure, special penalties ?
	return (sideToMove == black ? 1 : -1) * (material + mobility);
}

unsigned inline Board::blockedPawn(color col)
{
	// Returns number of blocked pawns.
	// Pawns can be blocked by pieces of any color
	if (col == black)
		return POPCOUNT((pieces[bp] << 8) & (whitePos | blackPos));
	else
		return POPCOUNT((pieces[wp] >> 8) & (blackPos | whitePos));
}
