#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
	entries.reserve(static_cast<size_t>(1e6));
	entries.resize(static_cast<size_t>(1e6));
	hashSize = static_cast<size_t>(1e6);
}

ZobristHash::ZobristHash(size_t _hashSize)
{
	entries.reserve(static_cast<u64>(_hashSize));
	entries.resize(_hashSize);
	hashSize = _hashSize;
}

ZobristHash::entry* const ZobristHash::addEntry(const u64 key, int value, int depth)
{
	auto index = key % hashSize;
	if (entries[index].search_depth < depth){
		entries[index].search_depth = depth;
		entries[index].value = value;
	}
	return &entries[index];
}

ZobristHash::entry& ZobristHash::getEntry(const u64 key)
{
	auto index = key % hashSize;
	return entries[index];
}

void inline ZobristHash::setBoundFlags(const u64 key, valueType vt) 
{
	auto index = key % hashSize;
	entries[index].flags = (entries[index].flags & 0x00FF) | vt;
}

// struct entry

ZobristHash::entry::entry() : value(0), search_depth(-1), flags(0x0) {}