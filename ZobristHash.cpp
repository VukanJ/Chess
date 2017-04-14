#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
	entries.reserve(static_cast<size_t>(1e6));
	entries.resize(static_cast<size_t>(1e6));
	hashSize = static_cast<size_t>(1e6);
}

ZobristHash::ZobristHash(size_t _hashSize)
{
	entries.reserve(static_cast<U64>(_hashSize));
	entries.resize(_hashSize);
	hashSize = _hashSize;
}

ZobristHash::entry* const ZobristHash::addEntry(const U64 key, int value, int depth)
{
	auto index = key % hashSize;
	if (entries[index].search_depth < depth){
		entries[index].search_depth = depth;
		entries[index].value = value;
	}
	return &entries[index];
}

ZobristHash::entry& ZobristHash::getEntry(const U64 key)
{
	auto index = key % hashSize;
	return entries[index];
}

int ZobristHash::getValue(const U64 key) const
{
	auto index = key % hashSize;
	return entries[index].value;
}

// Probably not needed:
void inline ZobristHash::setBoundFlags(const U64 key, valueType vt) 
{
	auto index = key % hashSize;
	entries[index].flags = vt;
}

// struct entry

ZobristHash::entry::entry() : value(-oo), search_depth(-1), flags(0x0) {}