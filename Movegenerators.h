#ifndef MOVEGENERATORS_H
#define MOVEGENERATORS_H
// Extends Board.h

template<moveGenType genType> void Board::generateMoveList(MoveList& moveList, color side)
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
	if (side == black) {
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
				case bp: pawnMoves<genType, black>(moveList); break;
				case br: rookMoves<genType, black>(moveList); break;
				case bn: knightMoves<genType, black>(moveList); break;
				case bb: queen_and_bishopMoves<genType, black, false>(moveList); break;
				case bq: queen_and_bishopMoves<genType, black, true>(moveList); break;
				case bk: kingMoves<genType, black>(moveList); break;
				}
			}
		}
		if (genType != CAPTURES_ONLY) {
			// Generate castling moves
			// Black King can castle if there are no pieces between king and rook, both havent moved yet and king
			// does not cross attacked squares during castling, same for white
			if ((castlingRights & castle_k) && !(allPos & 0x600000000000000ull) && !(whiteAtt & 0xE00000000000000ull)) {
				moveList.emplace_back(castlingRights, BCASTLE);
			}
			if ((castlingRights & castle_q) && !(allPos & 0x7000000000000000ull) && !(whiteAtt & 0x3800000000000000ull)) { // Black King can castle (big)
				moveList.emplace_back(castlingRights, BCASTLE_2);
			}
		}
	}
	else {
		/*
		::::::::::::::::::::::::::::::::::
		::     WHITE MOVE GENERATION    ::
		::::::::::::::::::::::::::::::::::
		*/
		wasInCheck = (pieces[wk] & blackAtt) > 0;
		for_white(w) { // Loop through white pieces
			attackingPieces = pieces[w];
			if (attackingPieces) {
				switch (w) {
				case wp: pawnMoves<genType, white>(moveList); break;
				case wr: rookMoves<genType, white>(moveList); break;
				case wn: knightMoves<genType, white>(moveList); break;
				case wb: queen_and_bishopMoves<genType, white, false>(moveList); break;
				case wq: queen_and_bishopMoves<genType, white, true>(moveList); break;
				case wk: kingMoves<genType, white>(moveList); break;
				}
			}
		}
		if (genType != CAPTURES_ONLY) {
			//  Castling permission King-rook path is not obstructed and not under attack
			if (castlingRights & castle_K && !(allPos & 0x6ull) && !(blackAtt & 0xEull)) { // White King can castle
				moveList.emplace_back(castlingRights, WCASTLE);
			}
			if (castlingRights & castle_Q && !(allPos & 0x70ull) && !(blackAtt & 0x38ull)) { // White King can castle queenside
				moveList.emplace_back(castlingRights, WCASTLE_2);
			}
		}
	}
	// If opponent rook has been captured, he looses castling rights.
	// TODO: Needs nicer solution

	//if (isKingInCheck(side))
	//	reduceMoveList(moveList, side);

	if (castlingRights & (side == black ? 0b1100 : 0b0011)) {
		for (auto& move : moveList) {
			if (move.targetPiece() == wr) {
				if (move.mtype() == CAPTURE || move.mtype() == C_PROMOTION) {
					if (move.to == a1)
						move.flags |= (castlingRights & castle_Q) << 4;
					else if (move.to == h1)
						move.flags |= (castlingRights & castle_K) << 4;
				}
			}
			else if (move.targetPiece() == br) {
				if (move.mtype() == CAPTURE || move.mtype() == C_PROMOTION) {
					if (move.to == a8)
						move.flags |= (castlingRights & castle_q) << 4;
					else if (move.to == h8)
						move.flags |= (castlingRights & castle_k) << 4;
				}
			}
		}
	}
}

template<moveGenType mgt, color side>
void inline Board::pawnMoves(MoveList& moveList) const
{
	U64 attackMask = 0x0, pieceAttacks = 0x0;
	piece p = side == white ? wp : bp;
	U64 attackingPieces = pieces[p];

	if(side == black){
		if (mgt & CAPTURES_ONLY) {
			// Find normal captures:
			for_bits(pos, attackingPieces) {
				attackMask = ((bit_at(pos) >> 9) & ~_left)  & whitePos;
				attackMask |= ((bit_at(pos) >> 7) & ~_right) & whitePos;

				for_white(candidate) {
					pieceAttacks = pieces[candidate] & attackMask;

					for_bits(target, pieceAttacks) {
						if (target < 8) {
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, bq));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, bn));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, br));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, bb));
						}
						else {
							moveList.emplace_back(pos, target, CAPTURE, piece_pair(bp, candidate));
						}
					}

				}
			}
			// Enpassent
			if (b_enpassant) {
				// There surely exists an enpassant move
				if ((bit_at(24 + b_enpassant) & pieces[bp]) & (_row << 24)) {
					// black pawn right of ep square
					moveList.emplace_back(24 + b_enpassant, 15 + b_enpassant, ENPASSANT, bp);
				}
				if (bit_at(22 + b_enpassant) & pieces[bp] & (_row << 24)) {
					// black pawn left of ep square
					moveList.emplace_back(22 + b_enpassant, 15 + b_enpassant, ENPASSANT, bp);
				}
			}
		}
		if (mgt & QUIET_ONLY) {
			// Find normal upwards moves and double pawn steps:
			attackingPieces = (pieces[bp] >> 8) & bpMove;
			for_bits(pos, attackingPieces) {
				if (pos > 7) {
					moveList.emplace_back(pos + 8, pos, MOVE, bp);
				}
				else {
					moveList.emplace_back(pos + 8, pos, PROMOTION, piece_pair(bp, bq));
					moveList.emplace_back(pos + 8, pos, PROMOTION, piece_pair(bp, bn));
					moveList.emplace_back(pos + 8, pos, PROMOTION, piece_pair(bp, br));
					moveList.emplace_back(pos + 8, pos, PROMOTION, piece_pair(bp, bb));
				}
			}
			// Double pawn move
			attackingPieces = ((((0x00FF000000000000 & pieces[bp]) >> 8) & bpMove) >> 8) & bpMove;
			for_bits(pos, attackingPieces) {
				moveList.emplace_back(pos + 16, pos, PAWN2, bp);
			}
		}
	}
	else {
		if (mgt & CAPTURES_ONLY) {
			// Find normal captures:
			// attackingPieces stands for attacked squares in this case
			for_bits(pos, attackingPieces) {
				attackMask  = (bit_at(pos) << 9 & ~_right) & blackPos;
				attackMask |= (bit_at(pos) << 7 & ~_left)  & blackPos;

				for_black(candidate) {
					pieceAttacks = pieces[candidate] & attackMask;
					for_bits(target, pieceAttacks) {
						if (target > 55) {
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, wq));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, wn));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, wr));
							moveList.emplace_back(pos, target, C_PROMOTION, piece_pair(candidate, wb));
						}
						else {
							moveList.emplace_back(pos, target, CAPTURE, piece_pair(wp, candidate));
						}
					}
				}
			}
			if (w_enpassant) {
				// There surely exists an enpassant move
				if ((bit_at(30 + w_enpassant) & pieces[wp]) & (_row << 32)) {
					// white pawn right of ep square
					moveList.emplace_back(30 + w_enpassant, 39 + w_enpassant, ENPASSANT, wp);
				 }
				 if (bit_at(32 + w_enpassant) & pieces[wp] & (_row << 32)) {
					// white pawn left of ep square
					moveList.emplace_back(32 + w_enpassant, 39 + w_enpassant, ENPASSANT, wp);
				}
			}
		}
		if (mgt & QUIET_ONLY) {
			// Find normal upwards moves and double pawn steps:
			attackingPieces = (pieces[wp] << 8) & wpMove;
			for_bits(pos, attackingPieces) {
				if (pos < 56) {
					moveList.emplace_back(pos - 8, pos, MOVE, wp);
				}
				else {
					moveList.emplace_back(pos - 8, pos, PROMOTION, piece_pair(wp, wq));
					moveList.emplace_back(pos - 8, pos, PROMOTION, piece_pair(wp, wn));
					moveList.emplace_back(pos - 8, pos, PROMOTION, piece_pair(wp, wr));
					moveList.emplace_back(pos - 8, pos, PROMOTION, piece_pair(wp, wb));
				}
			}
			attackingPieces = ((((0xFF00 & pieces[wp]) << 8) & wpMove) << 8) & wpMove;
			for_bits(pos, attackingPieces) {
				moveList.emplace_back(pos - 16, pos, PAWN2, wp);
			}
		}
	}
}


template<moveGenType mgt, color side>
void inline Board::knightMoves(MoveList& moveList) const
{
	U64 attackingPieces = pieces[side == white ? wn : bn];
	piece p = side == white ? wn : bn;

	U64 attackMask = 0x0, pieceAttacks = 0x0;
	for_bits(pos, attackingPieces) {
		attackMask = KNIGHT_ATTACKS[pos] & (side == black ? whitePos : blackPos);
		if (mgt & CAPTURES_ONLY) {
			for_color(candidate, !side) {
				pieceAttacks = pieces[candidate] & attackMask;
				if (pieceAttacks) {
					for_bits(target, pieceAttacks) {
						moveList.emplace_back(pos, target, CAPTURE, piece_pair(p, candidate));
					}
				}
			}
		}
		if (mgt & QUIET_ONLY) {
			attackMask ^= KNIGHT_ATTACKS[pos] & attacks[p];
			for_bits(target, attackMask) {
				moveList.emplace_back(pos, target, MOVE, p);
			}
		}
	}
}


template<moveGenType mgt, color side, bool Q>
void inline Board::queen_and_bishopMoves(MoveList& moveList) const
{
	U64 attackMask = 0x0, pieceAttacks = 0x0;
	piece p = Q ? (side == white ? wq : bq) : (side == white ? wb : bb);
	U64 attackingPieces = pieces[p];

	for_bits(pos, attackingPieces) {
		attackMask = (Q ? QUEEN_ATTACKS : BISHOP_ATTACKS)[pos] & attacks[p] & (side == black ? whitePos : blackPos);
		if (mgt & CAPTURES_ONLY) {
			for_color(candidate, !side) {
				pieceAttacks = pieces[candidate] & attackMask;
				if (pieceAttacks) {
					for_bits(target, pieceAttacks) {
						if (!(CONNECTIONS[pos][target] & allPos)) {
							moveList.emplace_back(pos, target, CAPTURE, piece_pair(p, candidate));
						}
					}
				}
			}
		}
		if(mgt & QUIET_ONLY){
			attackMask ^= (Q ? QUEEN_ATTACKS : BISHOP_ATTACKS)[pos] & attacks[p];
			for_bits(target, attackMask) {
				//moveList.reserve(moveList.size() + popcount(attackMask));
				if (!(CONNECTIONS[pos][target] & allPos)) {
					moveList.emplace_back(pos, target, MOVE, p);
				}
			}
		}
	}
}

template<moveGenType mgt, color side>
void inline Board::kingMoves(MoveList& moveList) const
{
	piece king = side == white ? wk : bk;
	U64 attackMask = 0x0, pieceAttacks = 0x0;
	ulong pos = msb(pieces[king]);
	attackMask = ((attacks[king] & (side == black ? whitePos : blackPos)) & ~(side == black ? whiteAtt : blackAtt));
	if (mgt & CAPTURES_ONLY) {
		for_color(candidate, !side) {
			pieceAttacks = pieces[candidate] & attackMask;
			if (pieceAttacks) {
				for_bits(target, pieceAttacks) {
					moveList.emplace_back(pos, target, move_metadata(CAPTURE, castlingRights & (side == black ? 0x3 : 0xC)), piece_pair(king, candidate));
				}
			}
		}
	}
	if (mgt & QUIET_ONLY) {
		attackMask ^= (KING_ATTACKS[pos] & attacks[king]) & ~(side == black ? whiteAtt : blackAtt);
		for_bits(target, attackMask) {
			moveList.emplace_back(pos, target, move_metadata(MOVE, castlingRights & (side == black ? 0x3 : 0xC)), king);
		}
	}
}

template<moveGenType mgt, color side>
void inline Board::rookMoves(MoveList& moveList) const
{
	piece rook = side == white ? wr : br;
	U64 attackingPieces = pieces[rook];
	U64 attackMask = 0x0, pieceAttacks = 0x0;
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
	for_bits(pos, attackingPieces) {
		attackMask = ROOK_ATTACKS[pos] & attacks[rook] & (side == black ? whitePos : blackPos);
		if (CAPTURES_ONLY) {
			if (attackMask) {
				for_color(candidate, !side) {
					pieceAttacks = pieces[candidate] & attackMask;

					for_bits(target, pieceAttacks) {
						if (!(CONNECTIONS[pos][target] & allPos)) {
							if (pos == a_square) {
								moveList.emplace_back(pos, target, move_metadata(CAPTURE, castlingRights & qCastRight), piece_pair(rook, candidate));
							}
							else if (pos == h_square) {
								moveList.emplace_back(pos, target, move_metadata(CAPTURE, castlingRights & kCastRight), piece_pair(rook, candidate));
							}
							else {
								moveList.emplace_back(pos, target, CAPTURE, piece_pair(rook, candidate));
							}
						}
					}
				}
			}
		}
		if (mgt & QUIET_ONLY) {
			attackMask ^= ROOK_ATTACKS[pos] & attacks[rook];
			for_bits(target, attackMask) {
				if (!(CONNECTIONS[pos][target] & allPos)) {
					if (pos == a_square) {
						moveList.emplace_back(pos, target, move_metadata(MOVE, castlingRights & qCastRight), rook);
					}
					else if (pos == h_square) {
						moveList.emplace_back(pos, target, move_metadata(MOVE, castlingRights & kCastRight), rook);
					}
					else {
						moveList.emplace_back(pos, target, MOVE, rook);
					}
				}
			}
		}
	}
}


#endif
