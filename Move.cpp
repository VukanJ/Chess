#include "Move.h"

Move::Move() : raw(0x0){}

Move::Move(U8 OldCastlingRights, U8 _flags)
{
	oldCastlingRights = OldCastlingRights;
	flags = _flags;
}

Move::Move(U8 _from, U8 _to, U8 _flags, U8 _pieces)
{
	from =_from;
	to =_to;
	flags =_flags;
	pieces =_pieces;
}

Move::Move(ulong _from, ulong _to, U8 _flags, U8 _pieces)
{
	from = _from;
	to = _to;
	flags =_flags;
	pieces = _pieces;
}

Move::Move(int _from, int _to, U8 _flags, U8 _pieces)
{
	from = _from;
	to = _to;
	flags = _flags;
	pieces = _pieces;
}
