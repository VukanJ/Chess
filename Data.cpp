#include "Data.h"

vector<vector<U64>> CONNECTIONS = vector<vector<U64>>(64, vector<U64>(64, 0x0));
vector<vector<U64>> magicRookMoveDatabase   = vector<vector<U64>>(64, vector<U64>());
vector<vector<U64>> magicBishopMoveDatabase = vector<vector<U64>>(64, vector<U64>());


void genChessData::genMoveData()
{
	genConnections();
	genMagicDatabases();
}

void genChessData::genMagicDatabases()
{
	// Generate variations
	auto rookOccVariations   = genOccupancyVariations(rookAttackMasks);
	auto bishopOccVariations = genOccupancyVariations(bishopAttackMasks);
	auto rookAttackSets      = genCorrectAttackSets(rookOccVariations, true);
	auto bishAttackSets      = genCorrectAttackSets(bishopOccVariations, false);

	// Initialize databases
	for (int m = 0; m < 64; ++m) {
		magicRookMoveDatabase[m]   = vector<U64>(rookOccVariations[m].size(), 0x0);
		magicBishopMoveDatabase[m] = vector<U64>(bishopOccVariations[m].size(), 0x0);
	}
	// Calculate databases for rooks
	U64 magicIndex = 0;
	for (int sq = 0; sq < 64; ++sq) {
		for (int i = 0; i < rookOccVariations[sq].size(); ++i) {
			magicIndex = (rookOccVariations[sq][i] * rookMagics[sq]) >> rookMagicShifts[sq];
			magicRookMoveDatabase[sq][magicIndex] = rookAttackSets[sq][i];
		}
	}
	// Calculate databases for bishops
	for (int sq = 0; sq < 64; ++sq) {
		for (int i = 0; i < bishopOccVariations[sq].size(); ++i) {
			magicIndex = (bishopOccVariations[sq][i] * bishopMagics[sq]) >> bishopMagicShifts[sq];
			magicBishopMoveDatabase[sq][magicIndex] = bishAttackSets[sq][i];
		}
	}
}

void genChessData::genConnections()
{
  cout << "Generating Bitboards...\n";
  // Generate rectangular rays
  vector<U64> rects(64, 0x0);
  for(int i = 0; i < 64; i++)
    rects[i] |= (col << (i % 8)) ^ (row << (i / 8) * 8);
  // Generate Diagnoal rays
  vector<U64> diags(64, 0x0);
  for(int i = 0; i < 64; i++){
    for(int j = 1; j < 8 - i % 8; j++)
      diags[i] |= (0x1ull << i) << j * 9;
    for(int j = 1; j < i % 8 + 1; j++)
      diags[i] |= (0x1ull << i) << j * 7;
    for(int j = 1; j < i % 8 + 1; j++)
      diags[i] |= (0x1ull << i) >> j * 9;
    for(int j = 1; j < 8 - i % 8; j++)
      diags[i] |= (0x1ull << i) >> j * 7;
  }
  U64 temp = 0x0;
  for(int i = 0; i < 64; i++){
    for(int j = 0; j < 64; j++){
      if(rects[j] & bit_at(i)){
        if(abs(i-j) < 8 && abs(i-j) > 1) // Pieces on same rank
          CONNECTIONS[i][j] = i < j ? (bit_at(j) - bit_at(i))^bit_at(i) : (bit_at(i) - bit_at(j))^bit_at(j);
        else if (j > i && j - i > 8){
          temp = bit_at(j) >> 8;
          while(!(temp & bit_at(i)))
            temp |= temp >> 8;
          CONNECTIONS[i][j] = temp ^ (bit_at(i));
        }
        else if (j < i && i - j > 8){
          temp = bit_at(j) << 8;
          while(!(temp & bit_at(i)))
            temp |= temp << 8;
          CONNECTIONS[i][j] = temp ^ (bit_at(i));
        }
      }
      else if(diags[j] & bit_at(i)){
        if(j / 8 > i / 8){
          if(j % 8 > i % 8 && j - i > 9){ // j above left of i
            temp = bit_at(j) >> 9;
            while(!(temp & bit_at(i)))
              temp |= temp >> 9;
            CONNECTIONS[i][j] = temp ^ (bit_at(i));
          }
          else if(j % 8 < i % 8 && j - i > 9){ // j above right of i
            temp = bit_at(j) >> 7;
            while(!(temp & bit_at(i)))
              temp |= temp >> 7;
            CONNECTIONS[i][j] = temp ^ (bit_at(i));
          }
        }
        else{
          if(j % 8 > i % 8 && i - j > 9){ // j below left of i
            temp = bit_at(j) << 7;
            while(!(temp & bit_at(i)))
              temp |= temp << 7;
            CONNECTIONS[i][j] = temp ^ (bit_at(i));
          }
          else if(j % 8 < i % 8 && i - j > 9){ // j below right of i
            temp = bit_at(j) << 9;
            while(!(temp & bit_at(i)))
              temp |= temp << 9;
            CONNECTIONS[i][j] = temp ^ (bit_at(i));
          }
        }
      }
    }
  }
}

pair<vector<U64>, vector<U64>> genChessData::genEffectiveAttacks()
{
	auto localRookAttackMask   = vector<U64>(64, 0);
	auto localBishopAttackMask = vector<U64>(BISHOP_ATTACKS.begin(), BISHOP_ATTACKS.end());

	// Generate relevant bishop masks:
	for (auto& ba : localBishopAttackMask) ba &= ~0xFF818181818181FFULL;
	// Generate relevant rook masks:
	for (int i = 0; i < 64; ++i) {
		localRookAttackMask[i] = (_right << (i % 8)) ^ (0xFFULL << ((i / 8) * 8));
		if (i % 8 != 0 && i % 8 != 7 && i / 8 != 0 && i / 8 != 7) {
			localRookAttackMask[i] &= ~0xFF818181818181FFULL;
		}
		else if (i / 8 == 0) { // first rank
			localRookAttackMask[i] &= ~0xFF00000000000000ULL;
			if (i % 8 == 0) { localRookAttackMask[i] &= ~0x80808080808080ULL; }
			else if (i % 8 == 7) { localRookAttackMask[i] &= ~0x01010101010101ULL; }
			else { localRookAttackMask[i] &= ~0x81818181818181ULL; }
		}
		else if (i / 8 == 7) { // last rank
			localRookAttackMask[i] &= ~0xFFULL;
			if (i % 8 == 0) { localRookAttackMask[i] &= ~0x8080808080808080ULL; }
			else if (i % 8 == 7) { localRookAttackMask[i] &= ~0x0101010101010101ULL; }
			else { localRookAttackMask[i] &= ~0x8181818181818181ULL; }
		}
		else if (i % 8 == 0) { localRookAttackMask[i] &= ~0xFF808080808080FFULL; }
		else if (i % 8 == 7) { localRookAttackMask[i] &= ~0xFF010101010101FFULL; }
	}
	return pair<vector<U64>, vector<U64>>(localRookAttackMask, localBishopAttackMask);
}

void genChessData::genMagic()
{
	// The following code is never executed at runtime
	auto att = genEffectiveAttacks();
	auto localRookAttackMask   = att.first;
	auto localBishopAttackMask = att.second;

	// Generate variations of occupancies since
	// many occupation variants correspond to the same attack set
	auto rookOccVariations   = genOccupancyVariations(localRookAttackMask);
	auto bishopOccVariations = genOccupancyVariations(localBishopAttackMask);
	auto rookAttackSets      = genCorrectAttackSets(rookOccVariations, true);
	auto bishAttackSets      = genCorrectAttackSets(bishopOccVariations, false);

	auto rookShifts = vector<U64>();
	auto bishShifts = vector<U64>();

	// Determine magic numbers by brute force:
	cout << "Rook Magics:\n" << string(80,'~') << endl;
	auto rookMagic   = generateMagicBitboards(rookOccVariations, rookAttackSets, localRookAttackMask);
	cout << "Bishop Magics:\n" << string(80, '~') << endl;
	auto bishopMagic = generateMagicBitboards(bishopOccVariations, bishAttackSets, localBishopAttackMask);
	cout << "Rook Shifts: \n" << string(80, '~') << endl;
	for (auto& r : localRookAttackMask) {
		cout << dec << (int)(64 - popcount(r)) << endl;
	}
	cout << "Bishop Shifts: \n" << string(80, '~') << endl;
	for (auto& r : localBishopAttackMask) {
		cout << dec << (int)(64 - popcount(r)) << endl;
	}

	cin.ignore();
}

vector<vector<U64>> genChessData::genCorrectAttackSets(vector<vector<U64>>& vars, bool isrook)
{
	// The following code is never executed at runtime
	U64 temp = 0;
	auto correctAttackSets = vector<vector<U64>>(64, vector<U64>());

	if (isrook) {
		for (int sq = 0; sq < 64; ++sq) {
			for (int i = 0; i < vars[sq].size(); ++i) {
				temp = 0;
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 0);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 1);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 2);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 3);
				correctAttackSets[sq].push_back(temp);
			}
		}
	}
	else {
		for (int sq = 0; sq < 64; ++sq) {
			for (int i = 0; i < vars[sq].size(); ++i) {
				temp = 0;
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 4);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 5);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 6);
				temp |= floodFill(bit_at(sq), ~vars[sq][i], 7);
				correctAttackSets[sq].push_back(temp);
			}
		}
	}

	return correctAttackSets;
}

U64 genChessData::floodFill(U64 propagator, U64 empty, int direction) const
{
	// The following code is never executed at runtime

	U64 flood = propagator;
	U64 wrap = noWrap[direction];
	empty &= wrap;
	auto r_shift = shift[direction];
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= propagator = rotate_l64(propagator, r_shift) & empty;
	flood |= rotate_l64(propagator, r_shift) & empty;
	return rotate_l64(flood, r_shift) & wrap;
}

vector<U64> genChessData::generateMagicBitboards(vector<vector<U64>>& vars, vector<vector<U64>>& correctAttacks, vector<U64>& masks)
{
	// The following code is never executed at runtime
	// Generates Magic numbers
	random_device r_device;
	mt19937_64 generator(r_device());
	generator.seed(42);
	uniform_int_distribution<U64> distr;

	vector<U64> finalMagicNumbers;

	bool fail = true;
	U64 magicNumber = 0x0;

	vector<U64> testHash; // For testing the mapping of the calculated magic numbers
	for (int sq = 0; sq < 64; sq++) {
		while(fail){
			// Generate random number with few set bits
			magicNumber = distr(generator) & distr(generator) & distr(generator);
			testHash = vector<U64>(correctAttacks[sq].size(), 0x0);
			fail = false;
			for (int i = 0; i < vars[sq].size() && !fail; ++i) {
				U64 magicIndex = (vars[sq][i] * magicNumber) >> (64 - popcount(masks[sq]));
				//printBitboard(magicIndex);
				// Check if magicIndex maps to wrong attack set
				fail = testHash[magicIndex] != 0 && testHash[magicIndex] != correctAttacks[sq][i];
				testHash[magicIndex] = correctAttacks[sq][i];
			}
		}
		fail = true;
		// Magic number was found!
		finalMagicNumbers.push_back(magicNumber);
		cout << "0x" << hex << magicNumber << ",\n";
	}
}

vector<vector<U64>> genChessData::genOccupancyVariations(vector<U64> occupancy)
{
	// The following code is never executed at runtime
	vector<vector<U64>> variations = vector<vector<U64>>(64, vector<U64>(1, 0x0));
	vector<int> bitIndex;

	uint counter = 0;
	uint squareCount = 0;
	U64 temp = 0;

	for (auto& occ : occupancy){
		// Generate bit indexing
		//printBitboard(occ);
		bitIndex.clear();
		BITLOOP(pos, occ) { bitIndex.push_back(pos); }
		uint maxCount = (0xFFFFFF >> (24 - bitIndex.size())) + 1;

		for (counter = 1; counter <= maxCount; ++counter) {
			// Map bits to a variation according to bitIndex
			temp = 0;
			//printBits(counter);
			for (uint r = 0x1, c = 0; c < bitIndex.size(); r <<= 1, c++) {
				if (r & counter) {
					temp |= 0x1ULL << (bitIndex[c]);
				}
			}
			if(temp) variations[squareCount].push_back(temp);
		}
		squareCount++;
	}
	return variations;
}

/*
void knight(vector<uint64>& nums)
{
	for (int i = 0; i < 64; i++) {
		nums[i] |= i < 20 ? (uint64)0xA1100110A << i >> 18 : (uint64)0xA1100110A << (i - 18);
		if (i % 8 < 3)
			nums[i] ^= nums[i] & _left;
		else if (i % 8 > 5)
			nums[i] ^= nums[i] & _right;
	}
}
*/
