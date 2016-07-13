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
		pieces[bp] = (u64)0xFF << 8;
		pieces[br] = (u64)0x81;
		pieces[bn] = (u64)0x42;
		pieces[bb] = (u64)0x24;
		pieces[bk] = (u64)0x10;
		pieces[bq] = (u64)0x8;
		pieces[wp] = (u64)0xFF << 48;
		pieces[wr] = (u64)0x81 << 56;
		pieces[wn] = (u64)0x42 << 56;
		pieces[wb] = (u64)0x24 << 56;
		pieces[wk] = (u64)0x10 << 56;
		pieces[wq] = (u64)0x8 << 56;
		blackPos = 0xFFFF;
		whitePos = 0xFFFF00000000;
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
					case 'p': pieces[bp] |= (u64)1 << (63 - counter); break;
					case 'r': pieces[br] |= (u64)1 << (63 - counter); break;
					case 'n': pieces[bn] |= (u64)1 << (63 - counter); break;
					case 'b': pieces[bb] |= (u64)1 << (63 - counter); break;
					case 'k': pieces[bk] |= (u64)1 << (63 - counter); break;
					case 'q': pieces[bq] |= (u64)1 << (63 - counter); break;
					case 'P': pieces[wp] |= (u64)1 << (63 - counter); break;
					case 'R': pieces[wr] |= (u64)1 << (63 - counter); break;
					case 'N': pieces[wn] |= (u64)1 << (63 - counter); break;
					case 'B': pieces[wb] |= (u64)1 << (63 - counter); break;
					case 'K': pieces[wk] |= (u64)1 << (63 - counter); break;
					case 'Q': pieces[wq] |= (u64)1 << (63 - counter); break;
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
	generateMoveList(movelist, black);


	for (auto& m : movelist){
		makeMove(m);
		cout << moveString(m) << endl;
		print();
		unMakeMove(m);
		//print();
	}

	cout << hashKey << endl;
	if (startingHash != hashKey){
		cerr << "\t\t\tHASHING ERROR\n" << endl;
	}
}

void Board::updateAllAttacks()
{
	for (int i = 0; i < 12; i++)
		updateAttack((piece)i);
	// Exclude pieces that attack pieces of same color
	for (int i = 0; i < 6; i++)
		attacks[i] = attacks[i] & ~blackPos;
	for (int i = 6; i < 12; i++)
		attacks[i] = attacks[i] & ~whitePos;
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
		case bk: break;
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
		case wk: break;
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
	// Vielleicht sollte ich diese Funktion löschen, und direkt in die Zuggeneration einfügen
	// Bei der Zug-Generation werden die Bitboards mit vorgefertigten boards ge-&-ndet.
	// Vielleicht sollten auch nur die Attacks bleiben (evaluation)
	if (side == black){
		attacks[bp] = 0x0;
		// Normal step
		attacks[bp] |= (pieces[bp] << 8) & ~(whitePos | blackPos);
		// Double step
		attacks[bp] |= ((((pieces[bp] & (u64)0xFF00) << 8) & ~(whitePos | blackPos)) << 8) & ~(whitePos | blackPos);
		// Side Attacks
		attacks[bp] |= ((pieces[bp] & _noSides)                << 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & _noSides)                << 7) & whitePos;
		attacks[bp] |= ((pieces[bp] & (u64)0x0101010101010101) << 9) & whitePos;
		attacks[bp] |= ((pieces[bp] & (u64)0x8080808080808080) << 7) & whitePos;
	}
	else{
		attacks[wp] = 0x0;
		// Normal step
		attacks[wp] |= (pieces[wp] >> 8) & ~(whitePos | blackPos);
		// Double step
		attacks[wp] |= ((((pieces[wp] & (u64)0xFF000000000000) >> 8) & ~(whitePos | blackPos)) >> 8) & ~(whitePos | blackPos);
		// Side Attacks
		attacks[wp] |= ((pieces[wp] & _noSides)                >> 9) & blackPos;
		attacks[wp] |= ((pieces[wp] & _noSides)                >> 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & (u64)0x0101010101010101) >> 7) & blackPos;
		attacks[wp] |= ((pieces[wp] & (u64)0x8080808080808080) >> 9) & blackPos;
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
					break;
				case br: // BLACK ROOK
					// Calculate attacked pieces
					BITLOOP(pos, attackingPieces){                                   // Loop through all positions of pieces of kind br
						tempMask = ((_col << pos % 8) ^ (_row << (pos / 8) * 8))&attacks[br] & whitePos; // Intersections with opponent pieces
						if (tempMask)                                                // If pieces are targeted
							WHITELOOP(candidate){                                        // Find targeted piece
							temp2 = pieces[candidate] & tempMask;                    // Set specific attacks
							if (temp2)
								BITLOOP(m, temp2)                                    // Add moves
								moveList.push_front(Move(pos, m, CAPTURE, (piece)b, (piece)candidate));
						}
						tempMask ^= ((_col << pos % 8) ^ (_row << (pos / 8) * 8))&attacks[br]; // Non capturing moves
						BITLOOP(m, tempMask)                                             // Add moves
							moveList.push_back(Move(pos, m, MOVE, (piece)b, nulPiece));
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
								moveList.push_front(Move(pos, m, CAPTURE, (piece)b, (piece)candidate));
						}
						tempMask ^= KNIGHT_ATTACKS[pos] & attacks[bn]; // Non capturing moves
						BITLOOP(m, tempMask)                        // Add moves
							moveList.push_back(Move(pos, m, MOVE, (piece)b,nulPiece));
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
								moveList.push_front(Move(pos, m, CAPTURE, (piece)b, (piece)candidate));
						}
						tempMask ^= BISHOP_ATTACKS[pos] & attacks[bb]; // Non capturing moves
						BITLOOP(m, tempMask)                        // Add moves
							moveList.push_back(Move(pos, m, MOVE, (piece)b, nulPiece));
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
								moveList.push_front(Move(pos, m, CAPTURE, (piece)b, (piece)candidate));
						}
						tempMask ^= QUEEN_ATTACKS[pos] & attacks[bq]; // Non capturing moves
						BITLOOP(m, tempMask)                        // Add moves
							moveList.push_back(Move(pos, m, MOVE, (piece)b, nulPiece));
					}
					break;
				case bk:
					break;
			}
		}
		// Generate castling moves
	if (castlingRights & Ck && !(blackPos & 0x60)) // Black King can castle
		moveList.push_back(Move(nulSq, nulSq, BCASTLE, nulPiece, nulPiece));
	if (castlingRights & CCk&& !(blackPos & 14)){ // Black King can castle (big)
		moveList.push_back(Move(nulSq, nulSq, BCASTLE_2, nulPiece, nulPiece));
	}
	}
}

void Board::makeMove(const Move& move)
{
	switch (move.flags){
		case MOVE:
			pieces[move.p] ^= (u64)0x1 << move.from;   // Piece disappears from departure
			pieces[move.p] |= (u64)0x1 << move.to;     // Piece appears at destination
			hashKey ^= randomSet[move.p][move.from];   // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			break;
		case CAPTURE:
			pieces[move.p] ^= (u64)0x1 << move.from;    // Piece disappears from departure
			pieces[move.p] |= (u64)0x1 << move.to;      // Piece appears at destination
			pieces[move.target] ^= (u64)0x1 << move.to; // Captured piece is deleted
			hashKey ^= randomSet[move.p][move.from];    // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			hashKey ^= randomSet[move.target][move.to];
			break;
		case BCASTLE: // Rochade
			makeMove(Move(d1, b1, MOVE, bk, nulPiece));
			makeMove(Move(a1, c1, MOVE, br, nulPiece));
			hashKey ^= randomSet[bk][d1];
			hashKey ^= randomSet[bk][b1];
			hashKey ^= randomSet[br][a1];
			hashKey ^= randomSet[br][c1];
			castlingRights ^= Ck;
			break;
		case BCASTLE_2: // Grand Rochade
			makeMove(Move(d1, f1, MOVE, bk, nulPiece));
			makeMove(Move(h1, e1, MOVE, br, nulPiece));
			hashKey ^= randomSet[bk][d1];
			hashKey ^= randomSet[bk][f1];
			hashKey ^= randomSet[br][h1];
			hashKey ^= randomSet[br][e1];
			castlingRights ^= CCk;
			break;
	}
}

void Board::unMakeMove(const Move& move)
{
	switch (move.flags){
		case MOVE:
			pieces[move.p] ^= (u64)0x1 << move.to;     // Piece disappears from destination
			pieces[move.p] |= (u64)0x1 << move.from;   // Piece appears at departure
			hashKey ^= randomSet[move.p][move.from];   // Update hashKey...
			hashKey ^= randomSet[move.p][move.to];
			break;
		case CAPTURE:
			pieces[move.p] ^= (u64)0x1 << move.to;      // Piece disappears from destination
			pieces[move.p] |= (u64)0x1 << move.from;    // Piece appears at departure
			pieces[move.target] |= (u64)0x1 << move.to; // Captured piece reappears
			hashKey ^= randomSet[move.p][move.from];    // Update hashKey...
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