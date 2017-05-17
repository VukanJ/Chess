#include "Move.h"

Move::Move() : raw(0x0){}

Move::Move(byte OldCastlingRights, byte _flags)
{
	oldCastlingRights = OldCastlingRights;
	flags = _flags;
}

Move::Move(byte _from, byte _to, byte _flags, byte _pieces)
{
	from =_from;
	to =_to;
	flags =_flags;
	pieces =_pieces;
}

Move::Move(ulong _from, ulong _to, byte _flags, byte _pieces)
{
	from = _from;
	to = _to;
	flags =_flags;
	pieces = _pieces;
}

Move::Move(int _from, int _to, byte _flags, byte _pieces)
{
	from = _from;
	to = _to;
	flags = _flags;
	pieces = _pieces;
}
