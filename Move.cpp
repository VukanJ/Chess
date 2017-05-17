#include "Move.h"

Move::Move() 
	: from(nullSquare), to(nullSquare), flags(0x0), pieces(nullPiece) {}

Move::Move(byte OldCastlingRights, byte _flags) 
	: from(OldCastlingRights), to(nullSquare), flags(_flags), pieces(nullPiece) {}

Move::Move(byte _from, byte _to, byte _flags, byte _pieces) 
	: from(_from), to(_to), flags(_flags), pieces(_pieces) {}

Move::Move(ulong _from, ulong _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), pieces(_pieces) {}

Move::Move(int _from, int _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), pieces(_pieces) {}

