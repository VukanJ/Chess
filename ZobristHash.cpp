#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
	entries.resize(static_cast<size_t>(1e6));
	hashSize = static_cast<size_t>(1e6);
}

ZobristHash::ZobristHash(size_t _hashSize)
{
	entries.resize(_hashSize);
	hashSize = _hashSize;
}

ZobristHash::entry& ZobristHash::getEntry(const U64& key)
{
	//cout << hex << key % hashSize << dec << endl;
	return entries[key % hashSize];
}

int ZobristHash::getValue(const U64& key) const
{
	return entries[key % hashSize].value;
}

bool ZobristHash::isRepetition(const U64& key, int depth) const
{
	// Stored node was found again at greater depth => Repetition
	auto& entry = entries[key % hashSize];
	return entry.search_depth != -1 && entry.search_depth < depth;
}

void ZobristHash::clear()
{
	for (auto& entry : entries) {
		// Invalidate entry:
		entry.search_depth = -1;
	}
}

ZobristHash::entry::entry() : value(-oo), search_depth(-1), flags(0x0), terminal(0), d(0){}

// Class PVTable

PVTable::PVTable()
{
	pventries.resize(static_cast<size_t>(3e6));
	hashSize = static_cast<size_t>(3e6);
}

PVTable::PVTable(size_t size)
{
	pventries.resize(size);
	hashSize = size;
}

void PVTable::addPVMove(const U64& key, const Move& move)
{
	auto index = key % hashSize;
	pventries[index].bestmove = move;
}

PVTable::PVEntry& PVTable::operator[](const U64& key)
{
	return pventries[key % hashSize];
}

void PVTable::clear()
{
	for (auto& entry : pventries) 
		entry.bestmove = Move();
}