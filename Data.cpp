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
      if(rects[j] & BIT_AT(i)){
        if(abs(i-j) < 8 && abs(i-j) > 1) // Pieces on same rank
          CONNECTIONS[i][j] = i < j ? (BIT_AT(j) - BIT_AT(i))^BIT_AT(i) : (BIT_AT(i) - BIT_AT(j))^BIT_AT(j);
        else if (j > i && j - i > 8){
          temp = BIT_AT(j) >> 8;
          while(!(temp & BIT_AT(i)))
            temp |= temp >> 8;
          CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
        }
        else if (j < i && i - j > 8){
          temp = BIT_AT(j) << 8;
          while(!(temp & BIT_AT(i)))
            temp |= temp << 8;
          CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
        }
      }
      else if(diags[j] & BIT_AT(i)){
        if(j / 8 > i / 8){
          if(j % 8 > i % 8 && j - i > 9){ // j above left of i
            temp = BIT_AT(j) >> 9;
            while(!(temp & BIT_AT(i)))
              temp |= temp >> 9;
            CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
          }
          else if(j % 8 < i % 8 && j - i > 9){ // j above right of i
            temp = BIT_AT(j) >> 7;
            while(!(temp & BIT_AT(i)))
              temp |= temp >> 7;
            CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
          }
        }
        else{
          if(j % 8 > i % 8 && i - j > 9){ // j below left of i
            temp = BIT_AT(j) << 7;
            while(!(temp & BIT_AT(i)))
              temp |= temp << 7;
            CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
          }
          else if(j % 8 < i % 8 && i - j > 9){ // j below right of i
            temp = BIT_AT(j) << 9;
            while(!(temp & BIT_AT(i)))
              temp |= temp << 9;
            CONNECTIONS[i][j] = temp ^ (BIT_AT(i));
          }
        }
      }
    }
  }
}
