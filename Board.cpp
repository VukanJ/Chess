#include "Board.h"

Board::Board()
	: whitePos(0x0), blackPos(0x0), whiteAtt(0x0), blackAtt(0x0), hashKey(0x0),
	castlingRights(0xFF), b_enpassent(0x0), w_enpassent(0x0)
{
	pieces = vector<u64>(12, 0x0);
	attacks = vector<u64>(12, 0x0);
	randomSet = hash.getRandomSet();
}

Board::Board(string fen) : Board()
{
	if (fen == "*"){ // Standard starting position
		pieces[bp] = 0xFFull << 8;
		pieces[br] = 0x81ull;
		pieces[bn] = 0x42ull;
		pieces[bb] = 0x24ull;
		pieces[bk] = 0x10ull;
		pieces[bq] = 0x8ull;
		pieces[wp] = 0xFFull << 48;
		pieces[wr] = 0x81ull << 56;
		pieces[wn] = 0x42ull << 56;
		pieces[wb] = 0x24ull << 56;
		pieces[wk] = 0x10ull << 56;
		pieces[wq] = 0x8ull << 56;
		blackPos = 0xFFFFull;
		whitePos = 0xFFFF00000000ull;
	}
	else { // Setup board according to FEN
		int counter = -1;
		for (auto& p : fen) {
			if (isdigit(p)) {
				counter += p - 48;
			}
			else {
				counter++;
				switch (p) {
					case 'p': pieces[bp] |= 0x1ull << (63 - counter); break;
					case 'r': pieces[br] |= 0x1ull << (63 - counter); break;
					case 'n': pieces[bn] |= 0x1ull << (63 - counter); break;
					case 'b': pieces[bb] |= 0x1ull << (63 - counter); break;
					case 'k': pieces[bk] |= 0x1ull << (63 - counter); break;
					case 'q': pieces[bq] |= 0x1ull << (63 - counter); break;
					case 'P': pieces[wp] |= 0x1ull << (63 - counter); break;
					case 'R': pieces[wr] |= 0x1ull << (63 - counter); break;
					case 'N': pieces[wn] |= 0x1ull << (63 - counter); break;
					case 'B': pieces[wb] |= 0x1ull << (63 - counter); break;
					case 'K': pieces[wk] |= 0x1ull << (63 - counter); break;
					case 'Q': pieces[wq] |= 0x1ull << (63 - counter); break;
					case '/': counter--; break;
					default: cerr << "Bad FEN! (Board::Board())\n"; exit(1); break;
				}
			}
		}
		for (int p = 0; p < 6; p++)
			blackPos |= pieces[p];
		for (int p = 6; p < 12; p++)
			whitePos |= pieces[p];
	}
	updateAllAttacks();
	initHash();
	
	// debug
	auto startingHash = hashKey;
	list<Move> movelist;
	movelist.clear();
	print();
	generateMoveList(movelist, black);

	for (auto& m : movelist){
		makeMove(m);
		cout << moveString(m) << endl;
		//print();
		unMakeMove(m);
		//print();
	}
	if (isCheckMate(black))
		cout << "CHECKMATE FOR BLACK!\n";
	else if (isCheckMate(white))
		cout << "CHECKMATE FOR WHITE!\n";

	cout << hashKey << endl;
	if (startingHash != hashKey){
		cerr << "\t\t\tHASHING ERROR\n" << endl;
	}
}

void Board::updateAllAttacks()
{
	whiteAtt = blackAtt = 0x0;
	for (int i = 0; i < 12; i++)
		updateAttack((piece)i);
	// Exclude pieces that attack pieces of same color
	BLACKLOOP(i)
		attacks[i] = attacks[i] & ~blackPos;
	WHITELOOP(i)
		attacks[i] = attacks[i] & ~whitePos;
	BLACKLOOP(i)
		blackAtt |= attacks[i];
	WHITELOOP(i)
		whiteAtt |= attacks[i];
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
	// Vielleicht sollte ich diese Funktion l�schen, und direkt in die Zuggeneration einf�gen
	// Bei der Zug-Generation werden die Bitboards mit vorgefertigten boards ge-&-ndet.
	// Vielleicht sollten auch nur die Attacks bleiben (evaluation)
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
	auto pos = 0;
	for (int i = 0; i < 12; i++){
		for (auto p : pieces){
			BITLOOP(pos, p){
				hashKey ^= randomSet[i][pos];
			}
		}
	}
	cout << hex << hashKey << endl;
}

void Board::generateMoveList(list<Move>& moveList, color side) const
{
	/*
	This method generates a list of all possible moves for a player.
	Aggressive and special moves are generated first and preferably stored
	at the front of the list
	*/
	unsigned long pos = nulSq, m=nulSq;
	u64 tempMask = 0x0;
	u64 temp2 = 0x0,attackingPieces = 0x0;
	// Generate all capturing and normal moves
	if (side == black){
	BLACKLOOP(b){ // Loop through black pieces
		attackingPieces = pieces[b];
		switch (b){
			case bp: 
				// Find normal captures:
				// attackingPieces stands for attacked squares in this case
				BITLOOP(pos, attackingPieces){
					tempMask = (0x5ull << (pos - 1+8)) & whitePos;
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                        // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2){
							BITLOOP(m, temp2){                                    // Add moves
								if (m > 55){
									moveList.push_front(Move(pos, m, C_PROMOTION, (piece)candidate, bq));
									moveList.push_front(Move(pos, m, C_PROMOTION, (piece)candidate, bn));
								}
								else{
									moveList.push_front(Move(pos, m, CAPTURE, bp, (piece)candidate));
								}
							}
						}
					}
				}
				// Find normal upwards moves:
				attackingPieces = (pieces[bp] << 8) & ~(blackPos | whitePos);
				BITLOOP(pos, attackingPieces){
					if (pos < 56){
						moveList.push_back(Move(pos - 8, pos, MOVE, bp, nulPiece));
					} else{
						moveList.push_front(Move(pos - 8, pos, PROMOTION, bp, bq));
						moveList.push_front(Move(pos - 8, pos, PROMOTION, bp, bn));
					}
				}
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
							moveList.push_front(Move(pos, m, CAPTURE, br, (piece)candidate));
					}
					tempMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8))&attacks[br]; // Non capturing moves
					BITLOOP(m, tempMask)                                             // Add moves
						moveList.push_back(Move(pos, m, MOVE, br, nulPiece));
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
							moveList.push_front(Move(pos, m, CAPTURE, bn, (piece)candidate));
					}
					tempMask ^= KNIGHT_ATTACKS[pos] & attacks[bn]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bn, nulPiece));
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
							moveList.push_front(Move(pos, m, CAPTURE, bb, (piece)candidate));
					}
					tempMask ^= BISHOP_ATTACKS[pos] & attacks[bb]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bb, nulPiece));
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
							moveList.push_front(Move(pos, m, CAPTURE, bq, (piece)candidate));
					}
					tempMask ^= QUEEN_ATTACKS[pos] & attacks[bq]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bq, nulPiece));
				}
				break;
			case bk: // BLACK KING

				// Calculate attacked pieces
				BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind bn
					tempMask = KING_ATTACKS[pos] & attacks[bk] & whitePos; // Intersections with opponent pieces
					if (tempMask)                                                // If pieces are targeted
						WHITELOOP(candidate){                                    // Find targeted piece
						temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
						if (temp2)
							BITLOOP(m, temp2)                                    // Add moves
							moveList.push_front(Move(pos, m, CAPTURE, bk, (piece)candidate));
					}
					tempMask ^= KING_ATTACKS[pos] & attacks[bk]; // Non capturing moves
					BITLOOP(m, tempMask)                        // Add moves
						moveList.push_back(Move(pos, m, MOVE, bk, nulPiece));
				}
				break;
			}
		}
		// Generate castling moves (temporarily deactivated)
	//if (castlingRights & Ck && !(blackPos & 0x60)) // Black King can castle
	//	moveList.push_back(Move(nulSq, nulSq, BCASTLE, nulPiece, nulPiece));
	//if (castlingRights & CCk&& !(blackPos & 14)){ // Black King can castle (big)
	//	moveList.push_back(Move(nulSq, nulSq, BCASTLE_2, nulPiece, nulPiece));
	//}
	}
}

void Board::makeMove(const Move& move)
{
	switch (move.flags){
		case MOVE:
			pieces[move.p] ^= 0x1ull << move.from;   // Piece disappears from departure
			pieces[move.p] |= 0x1ull << move.to;     // Piece appears at destination
			hashKey ^= randomSet[move.p][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			break;
		case CAPTURE:
			pieces[move.p] ^= 0x1ull << move.from;    // Piece disappears from departure
			pieces[move.p] |= 0x1ull << move.to;      // Piece appears at destination
			pieces[move.target] ^= 0x1ull << move.to; // Captured piece is deleted
			hashKey ^= randomSet[move.p][move.from];  // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			hashKey ^= randomSet[move.target][move.to];
			break;
		case PROMOTION:
			pieces[move.p] ^= 0x1ull << move.from;      // removes pawn
			pieces[move.target] |= 0x1ull << move.to;   // New piece appears 
			hashKey ^= randomSet[move.p][move.from];    // Update hashKey...
			hashKey ^= randomSet[move.target][move.to];
			break;
		case C_PROMOTION:
			//Needs better solution
			pieces[(move.to>55?bp:wp)] ^= 0x1ull << move.from;     // removes pawn
			pieces[move.p]      ^= 0x1ull << move.to;              // Captured piece disappears
			pieces[move.target] |= 0x1ull << move.to;              // New piece appears 
			hashKey ^= randomSet[move.to>55 ? bp : wp][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			hashKey ^= randomSet[move.target][move.to];
			break;
		case BCASTLE: // Castling short
			makeMove(Move(d1, b1, MOVE, bk, nulPiece));
			makeMove(Move(a1, c1, MOVE, br, nulPiece));
			castlingRights ^= Ck;
			break;
		case BCASTLE_2: // Castling long
			makeMove(Move(d1, f1, MOVE, bk, nulPiece));
			makeMove(Move(h1, e1, MOVE, br, nulPiece));
			castlingRights ^= CCk;
			break;
	}
}

void Board::unMakeMove(const Move& move)
{
	switch (move.flags){
		case MOVE:
			pieces[move.p] ^= 0x1ull << move.to;     // Piece disappears from destination
			pieces[move.p] |= 0x1ull << move.from;   // Piece appears at departure
			hashKey ^= randomSet[move.p][move.from];   // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			break;
		case CAPTURE:
			pieces[move.p] ^= 0x1ull << move.to;      // Piece disappears from destination
			pieces[move.p] |= 0x1ull << move.from;    // Piece appears at departure
			pieces[move.target] |= 0x1ull << move.to; // Captured piece reappears
			hashKey ^= randomSet[move.p][move.from];    // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			hashKey ^= randomSet[move.target][move.to];
			break;
		case PROMOTION:
			pieces[move.p] |= 0x1ull << move.from;
			pieces[move.target] ^= 0x1ull << move.to;
			hashKey ^= randomSet[move.p][move.from];    // Update hashKey...
			hashKey ^= randomSet[move.target][move.to];
			break;
		case C_PROMOTION:
			//Needs better solution
			pieces[move.to>55 ? bp : wp] |= 0x1ull << move.from;   // pawn reappears
			pieces[move.p]               |= 0x1ull << move.to;     // Captured piece reappears
			pieces[move.target] ^= 0x1ull << move.to;              // promoted piece disappears
			hashKey ^= randomSet[move.to>55 ? bp : wp][move.from]; // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			hashKey ^= randomSet[move.target][move.to];
			break;
		case BCASTLE: // Maybe add additional switch for castling
			makeMove(Move(b1, d1, MOVE, bk, nulPiece));
			makeMove(Move(c1, a1, MOVE, br, nulPiece));
			hashKey ^= randomSet[bk][d1];
			hashKey ^= randomSet[bk][b1];
			hashKey ^= randomSet[br][a1];
			hashKey ^= randomSet[br][c1];
			castlingRights |= Ck;
			break;
		case BCASTLE_2:
			makeMove(Move(f1, d1, MOVE, bk, nulPiece));
			makeMove(Move(e1, h1, MOVE, br, nulPiece));
			hashKey ^= randomSet[bk][d1];
			hashKey ^= randomSet[bk][f1];
			hashKey ^= randomSet[br][h1];
			hashKey ^= randomSet[br][e1];
			castlingRights |= CCk;
			break;
	}
}

bool Board::isCheckMate(color side) const
{
	// Not yet tested
	unsigned long pos;
	u64 mask;
	if (!pieces[bk] || !pieces[wk]){
		cerr << "Missing Kings!!\n";
		return false;
	}
	if (side == black){
		mask = pieces[bk];
		BITSCANR64(pos, mask);
		return (KING_ATTACKS[pos] & whiteAtt == KING_ATTACKS[pos]) && (pieces[bk] & whiteAtt);
	}
	else{
		mask = pieces[wk];
		BITSCANR64(pos, mask);
		return (KING_ATTACKS[pos] & blackAtt == KING_ATTACKS[pos]) && (pieces[wk] & blackAtt);
	}
	return true;
}

void Board::print()
{
	// Print full chessboard with symbols and borders without attacked squares
	static vector<string> asciiBoard = vector<string>(8, string(8, ' '));
	for (auto& r : asciiBoard)
		for (auto& c : r)
			c = '.';
	for (int p = 0; p < 12; p++) {
		auto temp = pieces[p];
		auto count = -1;
		for (u64 b = _start; b != 0; b >>= 1) {
			count++;
			if (b & temp)
				asciiBoard[count / 8][count % 8] = names[p];
		}
	}
	cout << string(10, 220) << endl;
	for (auto r : asciiBoard) {
		cout << (char)219;
		for (auto c : r)
			cout << c;
		cout << (char)219 << endl;
	}
	cout << string(10, 223) << endl;
}