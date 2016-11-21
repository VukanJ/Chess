#include "Move.h"

Move::Move() 
	: from(0), to(0), flags(0), Pieces(nulPiece) {}

Move::Move(byte OldCastlingRights, byte _flags) 
	: from(OldCastlingRights), to(nulSq), flags(_flags), Pieces(nulPiece) {}

Move::Move(byte _from, byte _to, byte _flags, byte _pieces) 
	: from(_from), to(_to), flags(_flags), Pieces(_pieces) {}

Move::Move(unsigned long _from, unsigned long _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), Pieces(_pieces) {}

Move::Move(int _from, int _to, byte _flags, byte _pieces) 
	: from((byte)_from), to((byte)_to), flags(_flags), Pieces(_pieces) {}
