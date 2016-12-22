#include "Move.h"

Move::Move() 
	: from(nullSquare), to(nullSquare), flags(0), Pieces(nullPiece) {}

Move::Move(byte OldCastlingRights, byte _flags) 
	: from(OldCastlingRights), to(nullSquare), flags(_flags), Pieces(nullPiece) {}

Move::Move(byte _from, byte _to, byte _flags, byte _pieces) 
	: from(_from), to(_to), flags(_flags), Pieces(_pieces) {}

Move::Move(unsigned long _from, unsigned long _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), Pieces(_pieces) {}

Move::Move(int _from, int _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), Pieces(_pieces) {}
