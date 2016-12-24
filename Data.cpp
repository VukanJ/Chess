#include "Data.h"

vector<vector<u64>> CONNECTIONS = vector<vector<u64>>(64, vector<u64>(64, 0x0));

void genChessData::gen()
{
  genConnections();
}
	
void genChessData::genConnections()
{
  cout << "Generating Bitboards...\n";
  // Generate rectangular rays
  vector<u64> rects(64, 0x0);
  for(int i = 0; i < 64; i++)
    rects[i] |= (col << (i % 8)) ^ (row << (i / 8) * 8);
  // Generate Diagnoal rays
  vector<u64> diags(64, 0x0);
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
  u64 temp = 0x0;
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