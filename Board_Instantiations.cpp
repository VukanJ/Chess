#include "Board.h"

// Make & Unmake Move
template void Board::makeMove<HASH_ONLY>(const Move&, color);
template void Board::makeMove<POS_ONLY >(const Move&, color);
template void Board::makeMove<PROPER   >(const Move&, color);

template void Board::unMakeMove<HASH_ONLY>(const Move&, color);
template void Board::unMakeMove<POS_ONLY >(const Move&, color);
template void Board::unMakeMove<PROPER   >(const Move&, color);

// Move appending functions
// Pawns
template void Board::pawnMoves<CAPTURES_ONLY, white>(MoveList& moveList) const;
template void Board::pawnMoves<QUIET_ONLY,    white>(MoveList& moveList) const;
template void Board::pawnMoves<ALL,           white>(MoveList& moveList) const;
template void Board::pawnMoves<CAPTURES_ONLY, black>(MoveList& moveList) const;
template void Board::pawnMoves<QUIET_ONLY,    black>(MoveList& moveList) const;
template void Board::pawnMoves<ALL,           black>(MoveList& moveList) const;
// Knights
template void Board::knightMoves<CAPTURES_ONLY, white>(MoveList&) const;
template void Board::knightMoves<QUIET_ONLY,    white>(MoveList&) const;
template void Board::knightMoves<ALL,           white>(MoveList&) const;
template void Board::knightMoves<CAPTURES_ONLY, black>(MoveList&) const;
template void Board::knightMoves<QUIET_ONLY,    black>(MoveList&) const;
template void Board::knightMoves<ALL,           black>(MoveList&) const;
// Queens
template void Board::queen_and_bishopMoves<CAPTURES_ONLY, white, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<QUIET_ONLY,    white, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<ALL,           white, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<CAPTURES_ONLY, white, false>(MoveList&) const;
template void Board::queen_and_bishopMoves<QUIET_ONLY,    white, false>(MoveList&) const;
template void Board::queen_and_bishopMoves<ALL,           white, false>(MoveList&) const;
template void Board::queen_and_bishopMoves<CAPTURES_ONLY, black, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<QUIET_ONLY,    black, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<ALL,           black, true >(MoveList&) const;
template void Board::queen_and_bishopMoves<CAPTURES_ONLY, black, false>(MoveList&) const;
template void Board::queen_and_bishopMoves<QUIET_ONLY,    black, false>(MoveList&) const;
template void Board::queen_and_bishopMoves<ALL,           black, false>(MoveList&) const;
// Kings
template void Board::kingMoves<CAPTURES_ONLY, white>(MoveList& moveList) const;
template void Board::kingMoves<QUIET_ONLY,    white>(MoveList& moveList) const;
template void Board::kingMoves<ALL,           white>(MoveList& moveList) const;
template void Board::kingMoves<CAPTURES_ONLY, black>(MoveList& moveList) const;
template void Board::kingMoves<QUIET_ONLY,    black>(MoveList& moveList) const;
template void Board::kingMoves<ALL,           black>(MoveList& moveList) const;
// Rooks
template void Board::rookMoves<CAPTURES_ONLY, white>(MoveList& moveList) const;
template void Board::rookMoves<QUIET_ONLY,    white>(MoveList& moveList) const;
template void Board::rookMoves<ALL,           white>(MoveList& moveList) const;
template void Board::rookMoves<CAPTURES_ONLY, black>(MoveList& moveList) const;
template void Board::rookMoves<QUIET_ONLY,    black>(MoveList& moveList) const;
template void Board::rookMoves<ALL,           black>(MoveList& moveList) const;