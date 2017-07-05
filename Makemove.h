#ifndef MAKEMOVE_H
#define MAKEMOVE_H

template<makeMoveType mmt>
void Board::makeMove(const Move& move, color side)
{
	hashKey ^= sideToMoveMask;
	switch (move.mtype()) {
	case MOVE:
		// Piece disappears from from-square and appears at to-square:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.pieces][move.from] ^ randomSet[move.pieces][move.to];
		}
		// Update Hashkey
		if (mmt & POS_ONLY) {
			pieces[move.movePiece()] ^= bit_at(move.from) | bit_at(move.to);
			// update position mask
			(side == black ? blackPos : whitePos) ^= bit_at(move.from) | bit_at(move.to);
		}
		break;
	case CAPTURE:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.movePiece()][move.from] // Update hashKey...
				^ randomSet[move.movePiece()][move.to]
				^ randomSet[move.targetPiece()][move.to];
		}
		if (mmt & POS_ONLY) {
			pieces[move.movePiece()] ^= (bit_at(move.from) | bit_at(move.to));
			pieces[move.targetPiece()] ^= bit_at(move.to);    // Captured piece is deleted
																	 // Update position mask
			if (side == black) {
				blackPos ^= bit_at(move.from) | bit_at(move.to);
				whitePos ^= bit_at(move.to);
			}
			else {
				whitePos ^= bit_at(move.from) | bit_at(move.to);
				blackPos ^= bit_at(move.to);
			}
		}
		break;
	case PAWN2:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.pieces][move.from] // Update hashKey...
				^ randomSet[move.pieces][move.to]
				^ randomSet[ENPASSENT_HASH][move.from % 8];
		}
		if (mmt & POS_ONLY) {
			pieces[move.pieces] ^= (bit_at(move.from) | bit_at(move.to));     // Piece disappears from departure
																			  // update position mask
			side == black ? (blackPos = ((blackPos ^ bit_at(move.from)) | bit_at(move.to)))
				: (whitePos = ((whitePos ^ bit_at(move.from)) | bit_at(move.to)));
		}
		if (mmt == FULL) {
			// The other player can then sometimes perform enpassent (if other pawn is available)
			if (move.pieces == bp && (0x5ull << (move.to - 1)) & (_row << 32) & pieces[wp]) {
				w_enpassent = (move.from % 8) + 1;
				b_enpassent = 0; // Maybe unnecessary
			}
			else if (move.pieces == wp && (0x5ull << (move.to - 1)) & (_row << 24) & pieces[bp]) {
				b_enpassent = (move.from % 8) + 1;
				w_enpassent = 0;
			}
		}
		break;
	case PROMOTION:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.movePiece()][move.from] // Update hashKey...
				^ randomSet[move.targetPiece()][move.to];
		}
		if (mmt & POS_ONLY) {
			pieces[move.movePiece()] ^= bit_at(move.from);    // removes pawn
			pieces[move.targetPiece()] |= bit_at(move.to);    // New piece appears
											                // update position mask
			(side == black ? blackPos : whitePos) ^= bit_at(move.from) | bit_at(move.to);
		}
		break;
	case C_PROMOTION:
		// QN | Captured
		if (side == black) {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[bp][move.from]
					^ randomSet[move.movePiece()][move.to]
					^ randomSet[move.targetPiece()][move.to];
			}
			if (mmt & POS_ONLY) {
				pieces[bp] ^= bit_at(move.from);
				pieces[move.movePiece()] ^= bit_at(move.to);
				pieces[move.targetPiece()] |= bit_at(move.to);

				blackPos ^= bit_at(move.from) | bit_at(move.to);
				whitePos ^= bit_at(move.to);
			}
		}
		else {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[wp][move.from]
					^ randomSet[move.movePiece()][move.to]
					^ randomSet[move.targetPiece()][move.to];
			}
			if (mmt & POS_ONLY) {
				pieces[wp] ^= bit_at(move.from);
				pieces[move.movePiece()] ^= bit_at(move.to);
				pieces[move.targetPiece()] |= bit_at(move.to);

				whitePos ^= bit_at(move.from) | bit_at(move.to);
				blackPos ^= bit_at(move.to);
			}
		}
		break;
	case BCASTLE: // Kingside castling
		if (mmt & HASH_ONLY) {
			// Update hash
			castlingRights &= ~(castle_k | castle_q);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_k];
		}
		if (mmt & POS_ONLY) {
			pieces[bk] = bit_at(g8);
			pieces[br] ^= (bit_at(h8) | bit_at(f8));
			// Override position
			blackPos ^= blackPos & 0xF00000000000000ull;
			blackPos |= 0x600000000000000ull;
		}
		break;
	case WCASTLE: // Castling short
		if (mmt & HASH_ONLY) {
			// Update hash
			castlingRights &= ~(castle_K | castle_Q);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_K];
		}
		if (mmt & POS_ONLY) {
			pieces[wk] = bit_at(g1);
			pieces[wr] ^= (bit_at(h1) | bit_at(f1));
			// Override position
			whitePos ^= whitePos & 0xF;
			whitePos |= 0x6;
		}
		break;
	case BCASTLE_2: // Castling long
		if (mmt & HASH_ONLY) {
			// Update hash
			castlingRights &= ~(castle_k | castle_q);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_q];
		}
		if (mmt & POS_ONLY) {
			pieces[bk] = bit_at(c8);
			pieces[br] ^= (bit_at(a8) | bit_at(d8));
			// Override position
			blackPos ^= blackPos & 0xF800000000000000ull;
			blackPos |= 0x3000000000000000ull;
		}
		break;
	case WCASTLE_2: // Castling long
		if (mmt & HASH_ONLY) {
			// Update hash
			castlingRights &= ~(castle_K | castle_Q);
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
		}
		if (mmt & POS_ONLY) {
			pieces[wk] = bit_at(c1);
			pieces[wr] ^= (bit_at(a1) | bit_at(d1));
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x30ull;
		}
		break;
	case ENPASSENT:
		if (move.pieces == bp) {
			if (mmt & HASH_ONLY) {
				// Update hashkey
				hashKey ^= randomSet[bp][move.from]
					^ randomSet[bp][move.to]
					^ randomSet[wp][move.to + 8]
					^ randomSet[ENPASSENT_HASH][move.from % 8];
			}
			if (mmt & POS_ONLY) {
				// Update positions
				pieces[bp] ^= (bit_at(move.from) | bit_at(move.to));
				pieces[wp] ^= bit_at(move.to + 8);

				blackPos ^= bit_at(move.from) | bit_at(move.to);
				whitePos ^= bit_at(move.to + 8);
			}
			if (mmt == FULL) {
				// No more enpassent squares after enpassent
				b_enpassent = 0;
			}
		}
		else {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[wp][move.from]
					^ randomSet[wp][move.to]
					^ randomSet[bp][move.to - 8]
					^ randomSet[ENPASSENT_HASH][move.from % 8];
			}
			if (mmt & POS_ONLY) {
				pieces[wp] ^= (bit_at(move.from) | bit_at(move.to));
				pieces[bp] ^= bit_at(move.to - 8);

				whitePos ^= bit_at(move.from) | bit_at(move.to);
				blackPos ^= bit_at(move.to - 8);
			}
			if (mmt == FULL) {
				w_enpassent = 0;
			}
		}
		break;
	default:
		cerr << "Invalid move info encountered!\n";
		cin.ignore();
		exit(1);
	}

	if (mmt == FULL) {
		// No enpassent squares after any other move than double pawn push
		if (move.mtype() != PAWN2) {
			b_enpassent = w_enpassent = 0;
		}
		// Check if castling still permitted
		U8 cast = move.castlingRights();
		if (cast) {
			castlingRights &= ~cast;
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
		}
	}
	if (mmt & POS_ONLY) {
		allPos = blackPos | whitePos;
	}
}

template<makeMoveType mmt>
void Board::unMakeMove(const Move& move, color side)
{
	hashKey ^= sideToMoveMask;
	switch (move.mtype()) {
	case MOVE:
		if (mmt & HASH_ONLY) {
			// Update hashKey...
			hashKey ^= randomSet[move.pieces][move.from] ^ randomSet[move.pieces][move.to];
		}
		if (mmt & POS_ONLY) {
			// Piece disappears from to-square and appears at from-square:
			pieces[move.pieces] ^= (bit_at(move.to) | bit_at(move.from));

			// Update position mask
			(side == black ? blackPos : whitePos) ^= bit_at(move.from) | bit_at(move.to);
		}
		break;
	case CAPTURE:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.movePiece()][move.from]
				^ randomSet[move.movePiece()][move.to]
				^ randomSet[move.targetPiece()][move.to];
		}
		if (mmt & POS_ONLY) {
			pieces[move.movePiece()] ^= (bit_at(move.to) | bit_at(move.from));
			pieces[move.targetPiece()] |= bit_at(move.to);
			// Update position mask
			if (side == black) {
				blackPos ^= bit_at(move.to) | bit_at(move.from);
				whitePos |= bit_at(move.to);
			}
			else {
				whitePos ^= bit_at(move.to) | bit_at(move.from);
				blackPos |= bit_at(move.to);
			}
		}
		break;
	case PAWN2:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.pieces][move.to]  // Update hashKey...
				^ randomSet[move.pieces][move.from]
				^ randomSet[ENPASSENT_HASH][move.from % 8];
		}
		if (mmt & POS_ONLY) {
			pieces[move.pieces] ^= (bit_at(move.to) | bit_at(move.from));     // Piece disappears from destination
																			  // update position mask
			(side == black ? blackPos : whitePos) ^= bit_at(move.from) | bit_at(move.to);
		}
		if (mmt == FULL) {
			b_enpassent = w_enpassent = 0x0;
		}
		break;
	case PROMOTION:
		if (mmt & HASH_ONLY) {
			hashKey ^= randomSet[move.movePiece()][move.from]
				^ randomSet[move.targetPiece()][move.to];
		}
		if (mmt & POS_ONLY) {
			pieces[move.movePiece()] |= bit_at(move.from);
			pieces[move.targetPiece()] ^= bit_at(move.to);
			// update position mask
			(side == black ? blackPos : whitePos) ^= bit_at(move.from) | bit_at(move.to);
		}
		break;
	case C_PROMOTION:
		if (side == black) {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[bp][move.from]
					^ randomSet[move.movePiece()][move.to]
					^ randomSet[move.targetPiece()][move.to];
			}
			if (mmt & POS_ONLY) {
				pieces[bp] |= bit_at(move.from);
				pieces[move.movePiece()] |= bit_at(move.to);
				pieces[move.targetPiece()] ^= bit_at(move.to);

				blackPos ^= bit_at(move.from) ^ bit_at(move.to);
				whitePos |= bit_at(move.to);
			}
		}
		else {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[wp][move.from]
					^ randomSet[move.movePiece()][move.to]
					^ randomSet[move.targetPiece()][move.to];
			}
			if (mmt & POS_ONLY) {
				pieces[wp] |= bit_at(move.from);
				pieces[move.movePiece()] |= bit_at(move.to);
				pieces[move.targetPiece()] ^= bit_at(move.to);

				whitePos ^= bit_at(move.from) ^ bit_at(move.to);
				blackPos |= bit_at(move.to);
			}
		}
		break;
	case BCASTLE:
		if (mmt & HASH_ONLY) {
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_k];
		}
		if (mmt & POS_ONLY) {
			pieces[bk] = bit_at(e8);
			pieces[br] ^= (bit_at(f8) | bit_at(h8));
			// Override position
			blackPos ^= blackPos & 0xF00000000000000ull;
			blackPos |= 0x900000000000000ull;
		}
		if (mmt == FULL) {
			castlingRights = move.oldCastlingRights();
		}
		break;
	case WCASTLE: // Castling short
		if (mmt & HASH_ONLY) {
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_K];
		}
		if (mmt & POS_ONLY) {
			pieces[wk] = bit_at(e1);
			pieces[wr] ^= (bit_at(f1) | bit_at(h1));
			// Override position
			whitePos ^= whitePos & 0xF;
			whitePos |= 0x9;
		}
		if (mmt == FULL) {
			castlingRights = move.oldCastlingRights();
		}
		break;
	case BCASTLE_2:
		if (mmt & HASH_ONLY) {
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_q];
		}
		if (mmt & POS_ONLY) {
			pieces[bk] = bit_at(e8);
			pieces[br] ^= (bit_at(d8) | bit_at(a8));
			// Override position
			blackPos ^= blackPos & 0xF800000000000000ull;
			blackPos |= 0x8800000000000000ull;
		}
		if (mmt == FULL) {
			castlingRights = move.oldCastlingRights();
		}
		break;
	case WCASTLE_2: // Castling long
		if (mmt & HASH_ONLY) {
			// Update hash
			hashKey ^= randomSet[CASTLE_HASH][castlingRights]
				^ randomSet[CASTLE_POSITION_HASH][HASH_CASTLE_Q];
		}
		if (mmt & POS_ONLY) {
			pieces[wk] = bit_at(e1);
			pieces[wr] ^= (bit_at(d1) | bit_at(a1));
			// Override position
			whitePos ^= whitePos & 0xF8ull;
			whitePos |= 0x88ull;
		}
		if (mmt == FULL) {
			castlingRights = move.oldCastlingRights();
		}
		break;
	case ENPASSENT:
		if (move.movePiece() == bp) {
			if (mmt & HASH_ONLY) {
				// Update hashkey
				hashKey ^= randomSet[bp][move.from]
					^ randomSet[bp][move.to]
					^ randomSet[wp][move.to + 8]
					^ randomSet[ENPASSENT_HASH][move.from % 8];
			}
			if (mmt & POS_ONLY) {
				pieces[bp] ^= (bit_at(move.to) | bit_at(move.from));
				pieces[wp] |= bit_at(move.to + 8);

				blackPos ^= bit_at(move.to);
				blackPos |= bit_at(move.from);
				whitePos |= bit_at(move.to + 8);
			}
			if (mmt == FULL) {
				b_enpassent = (move.to % 8) + 1;
			}
		}
		else {
			if (mmt & HASH_ONLY) {
				hashKey ^= randomSet[wp][move.from]
					^ randomSet[wp][move.to]
					^ randomSet[bp][move.to - 8]
					^ randomSet[ENPASSENT_HASH][move.from % 8];
			}
			if (mmt & POS_ONLY) {
				pieces[wp] ^= (bit_at(move.to) | bit_at(move.from));
				pieces[bp] |= bit_at(move.to - 8);

				whitePos ^= bit_at(move.to);
				whitePos |= bit_at(move.from);
				blackPos |= bit_at(move.to - 8);
			}
			if (mmt == FULL) {
				w_enpassent = (move.to % 8) + 1;
			}
		}
		break;
	default:
		cerr << "Invalid move encountered!\n";
		exit(1);
	}
	if (mmt == FULL) {
		// restore some castling rights
		U8 cast = move.castlingRights();
		if (cast) {
			hashKey ^= randomSet[CASTLE_HASH][castlingRights];
			castlingRights |= cast;
		}
	}
	if (mmt & POS_ONLY) {
		allPos = blackPos | whitePos;
	}
}

#endif
