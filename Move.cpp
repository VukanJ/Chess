#include "Move.h"

Move::Move() 
{
	from = to = flags = nullSquare;
	pieces = nullPiece;
}

Move::Move(U8 OldCastlingRights, U8 _flags)
{
	from = OldCastlingRights;
	flags = _flags;
}

Move::Move(U8 _from, U8 _to, U8 _flags, U8 _pieces)
{
	from =_from;
	to =_to;
	flags =_flags;
	pieces =_pieces;
}

Move& Move::operator=(const Move& o)
{
	from = o.from;
	to = o.to;
	flags = o.flags;
	pieces = o.pieces;
	return *this;
}