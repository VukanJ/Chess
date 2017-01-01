#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
	entries.reserve(static_cast<size_t>(1e6));
	entries.resize(static_cast<size_t>(1e6));
	hashSize = (u64)1e6;
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

bool ZobristHash::hasEntry(const u64 key) const
{
	auto index = key % hashSize;
	return (entries[index].search_depth != -1) ? true : false;
}

ZobristHash::entry* const ZobristHash::hasBetterEntry(const u64 key, int depth)
{
	// Return true if hash contains entry with greater or equal depth
	auto index = key % hashSize;
	if(entries[index].search_depth > -1 && entries[index].search_depth >= depth){
		return &entries[index];
	}
	else return nullptr;
}

ZobristHash::entry* const ZobristHash::getEntry(const u64 key, int& value)
{
	auto index = key % hashSize;
	if (entries[index].search_depth != -1){
		value = entries[index].value;
		return &entries[index];
	}
	return nullptr;
}

ZobristHash::entry* const ZobristHash::getEntry(const u64 key, int& value, int& depth)
{
	auto index = key % hashSize;
	if (entries[index].search_depth != -1){
		value = entries[index].value;
		depth = entries[index].search_depth;
		return &entries[index];
	}
	return nullptr;
}

void inline ZobristHash::setBoundFlags(const u64 key, valueType vt) 
{
	auto index = key % hashSize;
	entries[index].flags = (entries[index].flags & 0x00FF) | vt;
}

// struct entry

ZobristHash::entry::entry() : value(0), search_depth(-1), flags(0x0) {}