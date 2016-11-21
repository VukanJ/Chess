#include "Board.h"

Board::Board()
	: whitePos(0x0), blackPos(0x0), whiteAtt(0x0), blackAtt(0x0), hashKey(0x0),
	castlingRights(0x0), b_enpassent(0x0), w_enpassent(0x0)
{
	pieces    = vector<u64>(12, 0x0);
	attacks   = vector<u64>(12, 0x0);
	randomSet = hash.getRandomSet();
	// White always makes the first move
	sideToMove = white;
}

Board::Board(string fen) : Board()
{
	setupBoard(fen);
	debug();
}

void Board::setupBoard(string FEN)
{
	// Sets up Board according to FEN
	// FEN = [position(white's perspective) sideToMove castlingrights enpassentSquares NofHalfMoves MoveNumber]
	if (FEN == "*") { // Standard starting position
		for (int i = 0; i < 12; i++)
			pieces[i] = standardPosition[i];
		blackPos = 0xFFFF000000000000ull;
		whitePos = 0xFFFFull;
		allPos = blackPos | whitePos;
		castlingRights = 0xFull;
		sideToMove = white;
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
					pieces[getPieceIndex(f)] |= BIT_AT_R(counter);
				}
			}
		}
		for (int p = 0; p < 6; p++) blackPos |= pieces[p];
		for (int p = 6; p < 12; p++) whitePos |= pieces[p];
		// Set castling rights
		sideToMove = fenArgs[1][0] == 'w' ? white : black;
		for (auto c : fenArgs[2]) {
			switch (c) {
			case 'k': castlingRights |= castle_k;  break;
			case 'K': castlingRights |= castle_K;  break;
			case 'q': castlingRights |= castle_q; break;
			case 'Q': castlingRights |= castle_Q; break;
			}
		}
		allPos = blackPos | whitePos;
	}
	print();
}

void Board::debug()
{
	//cout << "Castling rights -> ";
	//printBits(castlingRights);
	updateAllAttacks();
	initHash();

	// debug
	printf("blocked black pawns: %d\n", blockedPawn(black));
	printf("blocked white pawns: %d\n", blockedPawn(white));
	auto startingHash = hashKey;
	vector<Move> movelist;
	cout << "Board value: " << evaluate() << endl;
	movelist.clear();
	print();

	color debugPlayerColor = black;

	generateMoveList(movelist, sideToMove);

	cout << "Start hash " << hex << hashKey << endl;
	int count = 0;
	for (auto& m : movelist) {
		cout << moveString(m) << (count % 10 == 0 ? "\n" : "  ");
		count++;
		makeMove(m, sideToMove);
		//print();
		//printBitboard(whitePos);
		unMakeMove(m, sideToMove);
		//print();
		//printBitboard(whitePos);
	}

	cout << "\nEnd hash   " << hex << hashKey << endl;
	if (isCheckMate(black))
		cout << "CHECKMATE FOR BLACK!\n";
	else if (isCheckMate(white))
		cout << "CHECKMATE FOR WHITE!\n";

	if (startingHash != hashKey) {
		cerr << "\t\t\t::: HASHING ERROR :::\n";
		printBitboard(hashKey);
	}
	else clog << "::: HASH OK :::\n";
	if (!pieces[bk] || !pieces[wk])
		cerr << "Missing Kings!!\n";
	if (POPCOUNT(pieces[bk]) > 1 || POPCOUNT(pieces[wk]) > 1) {
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
				attacks[br] |= floodFill(pieces[br], ~allPos, (dir)i++);
			break;
		case bn:
			mask = pieces[bn];
			attacks[bn] = 0x0;
			BITLOOP(pos,mask) attacks[bn] |= KNIGHT_ATTACKS[pos] & ~blackPos;
			break;
		case bb:
			attacks[bb] = 0x0;
			for (int i = 4; i < 8;)
				attacks[bb] |= floodFill(pieces[bb], ~allPos, (dir)i++);
			break;
		case bk:
			// TODO: No bitloop needed
			mask = pieces[bk];
			attacks[bk] = 0x0;
			BITLOOP(pos, mask) attacks[bk] |= KING_ATTACKS[pos] & ~blackPos;
			break;
		case bq:
			attacks[bq] = 0x0;
			for (int i = 0; i < 8;)
				attacks[bq] |= floodFill(pieces[bq], ~allPos, (dir)i++);
			break;
		case wp:
			pawnFill(white);
			break;
		case wr:
			attacks[wr] = 0x0;
			for (int i = 0; i < 4;)
				attacks[wr] |= floodFill(pieces[wr], ~allPos, (dir)i++);
			break;
		case wn:
			mask = pieces[wn];
			attacks[wn] = 0x0;
			BITLOOP(pos, mask) attacks[wn] |= KNIGHT_ATTACKS[pos] & ~whitePos;
			break;
		case wb:
			attacks[wb] = 0x0;
			for (int i = 4; i < 8;)
				attacks[wb] |= floodFill(pieces[wb], ~allPos, (dir)i++);
			break;
		case wk:
			mask = pieces[wk];
			attacks[wk] = 0x0;
			BITLOOP(pos, mask) attacks[wk] |= KING_ATTACKS[pos] & ~whitePos;
			break;
		case wq:
			attacks[wq] = 0x0;
			for (int i = 0; i < 8;)
				attacks[wq] |= floodFill(pieces[wq], ~allPos, (dir)i++);
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
		attacks[bp] |= (pieces[bp] >> 8) & ~allPos;
		// Double step
		attacks[bp] |= ((((pieces[bp] & 0xFF000000000000ull) >> 8) & ~allPos) >> 8) & ~allPos;
		// Side Attacks
		attacks[bp] |= ((pieces[bp] & _noSides) >> 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & _noSides) >> 7) & whitePos;
		attacks[bp] |= ((pieces[bp] & _left)    >> 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & _right)   >> 7) & whitePos;
	}
	else{
		attacks[wp] = 0x0;
		// Normal step
		attacks[wp] |= (pieces[wp] << 8) & ~allPos;
		// Double step
		attacks[wp] |= ((((pieces[wp] & 0xFF00ull) << 8) & ~allPos) << 8) & ~allPos;
		// Side Attacks
		attacks[wp] |= ((pieces[wp] & _noSides) << 9) & blackPos;
		attacks[wp] |= ((pieces[wp] & _noSides) << 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & _right)    << 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & _left)   << 9) & blackPos;
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

void Board::generateMoveList(vector<Move>& moveList, color side) const
{
	/*
	This method generates a list of all possible moves for a player.
	Aggressive and special moves are generated first and preferably stored
	at the front of the movelist
	*/
	unsigned long pos = nulSq;
	u64 attackMask = 0x0;
	u64 pieceAttacks = 0x0, attackingPieces = 0x0;
	// Generate all capturing and normal moves
	if (side == black){ ////////////////////////////////////////////////BLACK MOVE GENERATION///////////////////////////////////////////////////
	BLACKLOOP(b){ // Loop through black pieces
		attackingPieces = pieces[b];
		if(attackingPieces) // Only consider non-empty boards
		switch (b){
			case bp:
				// Find normal captures:
				// attackingPieces stands for attacked squares in this case
				BITLOOP(pos, attackingPieces){
					attackMask = (0x5ull << (pos - 1 + 8)) & whitePos;
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks){
							BITLOOP(target, pieceAttacks){                                   // Add moves
								if (target > 55){
									moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, PIECE_PAIR(candidate, bq)));
									moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, PIECE_PAIR(candidate, bn)));
								}
								else{
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(bp, candidate)));
								}
							}
						}
					}
				}
				// Find normal upwards moves and double pawn steps:
				attackingPieces = (pieces[bp] << 8) & ~allPos;
				BITLOOP(pos, attackingPieces){
					if (pos < 56){
						moveList.push_back(Move(pos - 8, pos, MOVE, bp));
					}
					else{
						moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, PIECE_PAIR(bp, bq)));
						moveList.insert(moveList.begin(), Move(pos - 8, pos, PROMOTION, PIECE_PAIR(bp, bn)));
					}
				}
				// Double pawn move
				attackingPieces = (pieces[bp] << 16) & attacks[bp];
				BITLOOP(pos, attackingPieces)
					moveList.push_back(Move(pos - 16, pos, PAWN2, bp));
				// Enpassent
				if (b_enpassent) { // if enpassent is available possible
					// Check if pawns are able to do enpassent
					attackingPieces = (pieces[bp] << 1) & pieces[wp] & (_row << 32);
					// Are white pawns left of black pawns?
					if (attackingPieces) {
						// Add move
						BITSCANR64(pos, attackingPieces);
						if (BIT_AT(pos + 8) & allPos)
							moveList.insert(moveList.begin(), Move(pos, pos + 8, ENPASSENT, bp));
					}
					else{
						// Are white pawns right of black pawns?
						attackingPieces = (pieces[bp] >> 1) & pieces[wp] & (_row << 32);
						if (attackingPieces) {
							// Add move
							BITSCANR64(pos, attackingPieces);
							if (BIT_AT(pos + 8) & allPos)
								moveList.insert(moveList.begin(), Move(pos, pos + 8, ENPASSENT, bp));
						}
					}
				}
				break;
			case br: // BLACK ROOK
				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind br
					attackMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br] & whitePos; // Intersections with opponent pieces
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                        // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
								if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE | ((pos == 0 ? castle_k : castle_q) << 4), PIECE_PAIR(br, candidate)));
					}
					attackMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[br]; // Non capturing moves
					BITLOOP(target, attackMask)                                             // Add moves
						if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
							moveList.push_back(Move(pos, target, MOVE | ((pos == 0 ? castle_k : castle_q) << 4), br));
				}
				break;
			case bn: //// BLACK KNIGHT

				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bn
					attackMask = KNIGHT_ATTACKS[pos] & attacks[bn] & whitePos; // Intersections with opponent pieces
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
							moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(bn, candidate)));
					}
					attackMask ^= KNIGHT_ATTACKS[pos] & attacks[bn]; // Non capturing moves
					BITLOOP(target, attackMask)                        // Add moves
						moveList.push_back(Move(pos, target, MOVE, bn));
				}
				break;
			case bb: // BLACK BISHOP
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bb
					attackMask = BISHOP_ATTACKS[pos] & attacks[bb] & whitePos; // Intersections with opponent pieces
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
								if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(bb, candidate)));
					}
					attackMask ^= BISHOP_ATTACKS[pos] & attacks[bb]; // Non capturing moves
					BITLOOP(target, attackMask)                        // Add moves
						if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
							moveList.push_back(Move(pos, target, MOVE, bb));
				}
				break;
			case bq: // BLACK QUEEN
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bq
					attackMask = QUEEN_ATTACKS[pos] & attacks[bq] & whitePos; // Intersections with opponent pieces
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
								if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(bq, candidate)));
					}
					attackMask ^= QUEEN_ATTACKS[pos] & attacks[bq]; // Non capturing moves
					BITLOOP(target, attackMask)                        // Add moves
						if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
							moveList.push_back(Move(pos, target, MOVE, bq));
				}
				break;
			case bk: // BLACK KING

				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                             // Loop through all positions of pieces of kind bk
					attackMask = (KING_ATTACKS[pos] & attacks[bk] & whitePos) & ~whiteAtt; // Intersections with opponent pieces that would not place king in check
					if (attackMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
							moveList.insert(moveList.begin(), Move(pos, target, CAPTURE | ((castle_k | castle_q) << 4), PIECE_PAIR(bk, candidate)));
					}
					attackMask ^= (KING_ATTACKS[pos] & attacks[bk]) & ~whiteAtt; // Non capturing moves
					BITLOOP(target, attackMask)                        // Add moves
						moveList.push_back(Move(pos, target, MOVE | ((castle_k | castle_q) << 4), bk));
				}
				break;
			}
		}
		// Generate castling moves
		// Black King can castle if there are no pieces between king and rook, both havent moved yet and king
		// does not cross attacked squares during castling, same for white
		if (castlingRights & castle_k && !(allPos & 0x60ull) && !(whiteAtt & 0x70ull))
			moveList.push_back(Move(castlingRights, BCASTLE));
		if (castlingRights & castle_q && !(allPos & 0xEull) && !(whiteAtt & 0x1Cull)) // Black King can castle (big)
			moveList.push_back(Move(castlingRights, BCASTLE_2));
	}
 else{////////////////////////////////////////////////WHITE MOVE GENERATION///////////////////////////////////////////////////
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	WHITELOOP(w){ // Loop through white pieces
		 attackingPieces = pieces[w];
		 if(attackingPieces)
		 switch (w){
			 case wp:
				 // Find normal captures:
				 // attackingPieces stands for attacked squares in this case
				 BITLOOP(pos, attackingPieces){
					 attackMask = (0x5ull << (pos - 1 - 8)) & blackPos;
					 if (attackMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						 if (pieceAttacks){
							 BITLOOP(target, pieceAttacks){                                   // Add moves
								 if (target < 8){
									 moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, PIECE_PAIR(candidate, wq)|(0x1 << 8)));
									 moveList.insert(moveList.begin(), Move(pos, target, C_PROMOTION, PIECE_PAIR(candidate, wn)|(0x1 << 8)));
								 }
								 else{
									 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(wp,candidate)));
								 }
							 }
						 }
					 }
				 }
				 // Find normal upwards moves and double pawn steps:
				 attackingPieces = (pieces[wp] >> 8) & ~allPos;
				 BITLOOP(pos, attackingPieces){
					 if (pos > 7){
						 moveList.push_back(Move(pos + 8, pos, MOVE, wp));
					 }
					 else{
						 moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, PIECE_PAIR(wp, wq)));
						 moveList.insert(moveList.begin(), Move(pos + 8, pos, PROMOTION, PIECE_PAIR(wp, wn)));
					 }
				 }
				 attackingPieces = (pieces[wp] >> 16) & attacks[wp];
				 BITLOOP(pos, attackingPieces)
						 moveList.push_back(Move(pos + 16, pos, PAWN2, wp));
				 if (w_enpassent) { // if enpassent is available possible
				 // Check if pawns are able to do enpassent
					 attackingPieces = (pieces[wp] << 1) & pieces[bp] & (_row << 24);
					 // Are white pawns left of black pawns?
					 if (attackingPieces) {
						 // Add move
						 BITSCANR64(pos, attackingPieces);
						 if (BIT_AT(pos - 8) & allPos)
							moveList.insert(moveList.begin(), Move(pos, pos - 8, ENPASSENT, wp));
					 }
					 else {
						 // Are white pawns right of black pawns?
						 attackingPieces = (pieces[wp] >> 1) & pieces[bp] & (_row << 24);
						 if (attackingPieces) {
							 // Add move
							 BITSCANR64(pos, attackingPieces);
							 if (BIT_AT(pos - 8) & allPos)
								 moveList.insert(moveList.begin(), Move(pos, pos - 8, ENPASSENT, wp));
						 }
					 }
				 }
				 break;
			 case wr: // WHITE ROOK
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                                         // Loop through all positions of pieces of kind br
					 attackMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr] & blackPos; // Intersections with opponent pieces
					 if (attackMask)                                                    // If pieces are targeted
						 BLACKLOOP(candidate){                                        // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                        // Set specific attacks
						 if (pieceAttacks)
							 BITLOOP(target, pieceAttacks)                                        // Add moves..
								if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE | ((pos == 56 ? castle_K : castle_Q) << 4), PIECE_PAIR(wr, candidate)));
					 }
					 attackMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8)) & attacks[wr]; // Non capturing moves
					 //printBitboard(tempMask);
					 BITLOOP(target, attackMask)                                         // Add moves..
						 if(!(CONNECTIONS[pos][target] & allPos))       // ..if no piece is in the way
							moveList.push_back(Move(pos, target, MOVE | ((pos == 56 ? castle_K : castle_Q) << 4), wr));
				 }
				 break;
			 case wn: //// WHITE KNIGHT
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bn
					 attackMask = KNIGHT_ATTACKS[pos] & attacks[wn] & blackPos;     // Intersections with opponent pieces
					 if (attackMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						 if (pieceAttacks)
							BITLOOP(target, pieceAttacks)                                    // Add moves
								moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(wn, candidate)));
					 }
					 attackMask ^= KNIGHT_ATTACKS[pos] & attacks[wn]; // Non capturing moves
					 BITLOOP(target, attackMask)                        // Add moves
						moveList.push_back(Move(pos, target, MOVE, wn));
				 }
				 break;
			 case wb: // WHITE BISHOP
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bb
					 attackMask = BISHOP_ATTACKS[pos] & attacks[wb] & blackPos;     // Intersections with opponent pieces
					 if (attackMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						 if (pieceAttacks)
							 BITLOOP(target, pieceAttacks)                                    // Add moves
								 if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(wb, candidate)));
					 }
					 attackMask ^= BISHOP_ATTACKS[pos] & attacks[wb]; // Non capturing moves
					 BITLOOP(target, attackMask)                        // Add moves
						 if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
							 moveList.push_back(Move(pos, target, MOVE, wb));
				 }
				 break;
			 case wq: // WHITE QUEEN
				 BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bq
					 attackMask = QUEEN_ATTACKS[pos] & attacks[wq] & blackPos; // Intersections with opponent pieces
					 if (attackMask)                                                // If pieces are targeted
						 BLACKLOOP(candidate){                                    // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                    // Set specific attacks
						 if (pieceAttacks)
							 BITLOOP(target, pieceAttacks)                                    // Add moves
								if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
									moveList.insert(moveList.begin(), Move(pos, target, CAPTURE, PIECE_PAIR(wq, candidate)));
					 }
					 attackMask ^= QUEEN_ATTACKS[pos] & attacks[wq]; // Non capturing moves
					 BITLOOP(target, attackMask)                        // Add moves
						 if (!(CONNECTIONS[pos][target] & allPos))   // ..if no piece is in the way
							moveList.push_back(Move(pos, target, MOVE, wq));
				 }
				 break;
			 case wk: // WHITE KING
				 // Calculate attacked pieces
				 BITLOOP(pos, attackingPieces){                                           // Loop through all positions of pieces of kind wk
					 attackMask = (KING_ATTACKS[pos] & attacks[wk] & blackPos) & ~blackAtt; // Intersections with opponent pieces that would not place king in check
					 if (attackMask)                                                        // If pieces are targeted
						 BLACKLOOP(candidate){                                            // Find targeted piece
						 pieceAttacks = pieces[candidate] & attackMask;                            // Set specific attacks
						 if (pieceAttacks)
							 BITLOOP(target, pieceAttacks)                                            // Add moves
							 moveList.insert(moveList.begin(), Move(pos, target, CAPTURE | ((castle_K | castle_Q) << 4), PIECE_PAIR(wk, candidate)));
					 }
					 attackMask ^= (KING_ATTACKS[pos] & attacks[wk]) & ~blackAtt; // Non capturing moves
					 BITLOOP(target, attackMask)                         // Add moves
						 moveList.push_back(Move(pos, target, MOVE | ((castle_K | castle_Q) << 4), wk));
				 }
				 break;
		 }
	 }
	 // Es muss HIER ueberprueft werden, ob die Figuren existieren (logisch)
	 if (castlingRights & castle_K && !(allPos & 0x6000000000000000ull) && !(blackAtt & 0x7000000000000000ull)) // White King can castle
		moveList.push_back(Move(castlingRights, WCASTLE));
	 if (castlingRights & castle_Q && !(allPos & 0xE00000000000000ull) && !(blackAtt & 0x1C00000000000000ull)) // White King can castle (big)
		 moveList.push_back(Move(castlingRights, WCASTLE_2));
	}
}

bool Board::makeMove(const Move& move, color side)
{
	// Returns true if move is invalid (King is in check)

	switch (move.flags & 0xFull){
		case MOVE:
			pieces[move.Pieces] ^= BIT_AT(move.from);     // Piece disappears from departure
			pieces[move.Pieces] |= BIT_AT(move.to);       // Piece appears at destination
			hashKey ^= randomSet[move.Pieces][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.Pieces][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.from)) | BIT_AT(move.to)))
						  : (whitePos = ((whitePos ^ BIT_AT(move.from)) | BIT_AT(move.to)));
			break;
		case CAPTURE:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.from);     // Piece disappears from departure
			pieces[MOV_PIECE(move.Pieces)] |= BIT_AT(move.to);       // Piece appears at destination
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);    // Captured piece is deleted
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ BIT_AT(move.from)) | BIT_AT(move.to);
				whitePos ^= BIT_AT(move.to);
			}
			else {
				whitePos = (whitePos ^ BIT_AT(move.from)) | BIT_AT(move.to);
				blackPos ^= BIT_AT(move.to);
			}
			break;
		case PAWN2:
			pieces[move.Pieces] ^= BIT_AT(move.from);     // Piece disappears from departure
			pieces[move.Pieces] |= BIT_AT(move.to);       // Piece appears at destination
			hashKey ^= randomSet[move.Pieces][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.Pieces][move.to];
			hashKey ^= randomSet[ENPASSENT_HASH][move.from % 8];
			(move.Pieces == bp ? w_enpassent : b_enpassent) |= 0x1 << (move.from % 8); // The other player can then perform enpassent
			// update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.from)) | BIT_AT(move.to)))
				          : (whitePos = ((whitePos ^ BIT_AT(move.from)) | BIT_AT(move.to)));
			break;
		case PROMOTION:
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.from);     // removes pawn
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);    // New piece appears
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.from)) | BIT_AT(move.to)))
				          : (whitePos = ((whitePos ^ BIT_AT(move.from)) | BIT_AT(move.to)));
		case C_PROMOTION:
			pieces[((move.Pieces&(0x3ull << 8)) >> 8) * 6] ^= BIT_AT(move.from);     // removes pawn
			pieces[MOV_PIECE(move.Pieces)] ^= BIT_AT(move.to);                    // Captured piece disappears
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);                 // New piece appears
			hashKey ^= randomSet[((move.Pieces&(0x3ull << 8)) >> 8) * 6][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ BIT_AT(move.from)) | BIT_AT(move.to);
				whitePos ^= BIT_AT(move.to);
			}
			else {
				whitePos = (whitePos ^ BIT_AT(move.from)) | BIT_AT(move.to);
				blackPos ^= BIT_AT(move.to);
			}
			break;
		case BCASTLE: // Castling short
			castlingRights &= ~(castle_k | castle_q);                     // No castling rights after castling
			makeMove(Move(d1, b1, MOVE, bk), black);           // move king and rook...
			makeMove(Move(a1, c1, MOVE, br), black);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]; // update hashKey with new castling rights
			// Update position mask
			blackPos ^= (blackPos & 0xF0ull); // turn 10010000 into 01100000
			blackPos |= 0x60ull;
			break;
		case WCASTLE: // Castling short
			castlingRights &= ~(castle_K | castle_Q);
			makeMove(Move(d8, b8, MOVE, wk), white);
			makeMove(Move(a8, c8, MOVE, wr), white);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			// Update position mask
			whitePos ^= (whitePos & 0xF000000000000000ull);
			whitePos |= 0x6000000000000000ull;
			break;
		case BCASTLE_2: // Castling long
			castlingRights &= ~(castle_k | castle_q);
			makeMove(Move(d1, f1, MOVE, bk), black);
			makeMove(Move(h1, e1, MOVE, br), black);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			blackPos ^= (blackPos & 0x1Full); // turn 00010001 into 00001100
			blackPos |= 0xCull;
			break;
		case WCASTLE_2: // Castling long
			castlingRights &= ~(castle_K | castle_Q);
			makeMove(Move(d8, f8, MOVE, wk), white);
			makeMove(Move(h8, e8, MOVE, wr), white);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			whitePos ^= (whitePos & 0x1F00000000000000ull); // turn 00010001 into 00001100
			whitePos |= 0xC00000000000000ull;
			break;
		case ENPASSENT:
			if (move.Pieces == bp) {
				pieces[bp] ^= BIT_AT(move.from);
				pieces[bp] |= BIT_AT(move.to);
				pieces[wp] ^= BIT_AT(move.to + 8);
			}
			else {
				// WIP
			}
			break;
		default:
			cerr << "Invalid move encountered!\n";
			exit(1);
	}
	// Check if castling still permitted
	byte cast = (move.flags & 0xF0ull)>>4;
	if (cast){
		if (cast & castle_k)      { castlingRights &= ~castle_k;  }
		else if (cast & castle_q){ castlingRights &= ~castle_q; }
		if (cast & castle_K)      { castlingRights &= ~castle_K;  }
		else if (cast & castle_Q){ castlingRights &= ~castle_Q; }
	}
	allPos = blackPos | whitePos;
	if (((side == black) && (pieces[bk] & whiteAtt))
		|| ((side == white) && (pieces[wk] & blackAtt))) {
		return true; // Move is invalid, king left in check
	}
	else return false;
}

void Board::unMakeMove(const Move& move, color side)
{
	switch (move.flags & 0xFull){
		case MOVE:
			pieces[move.Pieces] ^= BIT_AT(move.to);       // Piece disappears from destination
			pieces[move.Pieces] |= BIT_AT(move.from);     // Piece reappears at departure
			hashKey ^= randomSet[move.Pieces][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.Pieces][move.to];
			// Update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.to)) | BIT_AT(move.from)))
						  : (whitePos = ((whitePos ^ BIT_AT(move.to)) | BIT_AT(move.from)));
			break;
		case CAPTURE:
			pieces[MOV_PIECE(move.Pieces)]    ^= BIT_AT(move.to);    // Piece disappears from destination
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.from);  // Piece appears at departure
			pieces[TARGET_PIECE(move.Pieces)] |= BIT_AT(move.to);    // Captured piece reappears
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ BIT_AT(move.to)) | BIT_AT(move.from);
				whitePos |= BIT_AT(move.to);
			}
			else {
				whitePos = (whitePos ^ BIT_AT(move.to)) | BIT_AT(move.from);
				blackPos |= BIT_AT(move.to);
			}
			break;
		case PAWN2:
			pieces[move.Pieces] ^= BIT_AT(move.to);       // Piece disappears from destination
			pieces[move.Pieces] |= BIT_AT(move.from);     // Piece appears at departure
			hashKey ^= randomSet[move.Pieces][move.to]; // Update hashKey...
			hashKey ^= randomSet[move.Pieces][move.from];
			hashKey ^= randomSet[ENPASSENT_HASH][move.from % 8];
			b_enpassent = w_enpassent = 0x0;
			// update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.to)) | BIT_AT(move.from)))
			              : (whitePos = ((whitePos ^ BIT_AT(move.to)) | BIT_AT(move.from)));
			break;
		case PROMOTION:
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.from);
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.from];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// update position mask
			side == black ? (blackPos = ((blackPos ^ BIT_AT(move.to)) | BIT_AT(move.from)))
				          : (whitePos = ((whitePos ^ BIT_AT(move.to)) | BIT_AT(move.from)));
			break;
		case C_PROMOTION:
			pieces[((move.Pieces&(0x3ull << 8)) >> 8) * 6] |= BIT_AT(move.from);     // Pawn reappears
			pieces[MOV_PIECE(move.Pieces)]    |= BIT_AT(move.to);                 // Captured piece reappears
			pieces[TARGET_PIECE(move.Pieces)] ^= BIT_AT(move.to);                 // Promoted piece disappears
			hashKey ^= randomSet[((move.Pieces&(0x3ull << 8)) >> 8) * 6][move.from]; // Update hashKey...
			hashKey ^= randomSet[MOV_PIECE(move.Pieces)][move.to];
			hashKey ^= randomSet[TARGET_PIECE(move.Pieces)][move.to];
			// Update position mask
			if (side == black) {
				blackPos = (blackPos ^ BIT_AT(move.to)) | BIT_AT(move.from);
				whitePos |= BIT_AT(move.to);
			}
			else {
				whitePos = (whitePos ^ BIT_AT(move.to)) | BIT_AT(move.from);
				blackPos |= BIT_AT(move.to);
			}
			break;
		case BCASTLE:
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]; // Reapply new castling rights to hash (inverse operation)
			makeMove(Move(b1, d1, MOVE, bk), black);
			makeMove(Move(c1, a1, MOVE, br), black);
			castlingRights = move.from;                        // Restore old rights

			blackPos ^= (blackPos & 0xF0ull); // turn 01100000 into 10010000
			blackPos |= 0x90ull;
			break;
		case WCASTLE: // Castling short
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(b8, d8, MOVE, wk), white);
			makeMove(Move(c8, a8, MOVE, wr), white);
			castlingRights = move.from;
			// Update position mask
			whitePos ^= (whitePos & 0xF000000000000000ull);
			whitePos |= 0x9000000000000000ull;
			break;
		case BCASTLE_2:
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(f1, d1, MOVE, bk), black);
			makeMove(Move(e1, h1, MOVE, br), black);
			castlingRights = move.from;

			blackPos ^= (blackPos & 0xFull); // turn 00001100 into 00010001
			blackPos |= 0x11ull;
			break;
		case WCASTLE_2: // Castling long
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			makeMove(Move(f8, d8, MOVE, wk), white);
			makeMove(Move(e8, h8, MOVE, wr), white);
			castlingRights = move.from;

			whitePos ^= (whitePos & 0xF00000000000000ull); // turn 00001100 into 00010001
			whitePos |= 0x1100000000000000ull;
			break;
		case ENPASSENT:
			break;
		default:
			cerr << "Invalid move encountered!\n";
			exit(1);
	}
	allPos = blackPos | whitePos;
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
			return ((KING_ATTACKS[pos] & whiteAtt) == KING_ATTACKS[pos]) && (pieces[bk] & whiteAtt);
		}
		else return false;
	}
	else{
		if (blackAtt & pieces[bk]){
			mask = pieces[wk];
			BITSCANR64(pos, mask);
			return ((KING_ATTACKS[pos] & blackAtt) == KING_ATTACKS[pos]) && (pieces[wk] & blackAtt);
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

float Board::evaluate()
{
	// Computer is always maximizing player

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
	mobility *= 0.1f;
	// WIP: King safety, pawn structure, special penalties ?
	return (sideToMove == black ? 1 : -1) * (material + mobility);
}

unsigned inline Board::blockedPawn(color col)
{
	// Returns number of blocked pawns.
	// Pawns can be blocked by pieces of any color
	if (col == black)
		 return (unsigned) POPCOUNT((pieces[bp] << 8) & allPos);
	else return (unsigned) POPCOUNT((pieces[wp] >> 8) & allPos);
}
