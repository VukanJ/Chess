#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace std;

typedef unsigned long long uint64;

const uint64 col    = 0x101010101010101;
const uint64 row    = 0xFF;
const uint64 _right = 0x0303030303030303;
const uint64 _left  = 0xC0C0C0C0C0C0C0C0;

#define BIT(x) (0x1ull << x)

void diags(vector<uint64>& nums)
{
  for(int i = 0; i < 64; i++){
    for(int j = 1; j < 8-i%8; j++)
      nums[i] |= ((uint64) 0x1 << i) << j*9;
    for(int j = 1; j < i%8+1; j++)
      nums[i] |= ((uint64) 0x1 << i) << j*7;
    for(int j = 1; j < i%8+1; j++)
      nums[i] |= ((uint64) 0x1 << i) >> j*9;
    for(int j = 1; j < 8-i%8; j++)
      nums[i] |= ((uint64) 0x1 << i) >> j*7;
  }
}

void rects(vector<uint64>& nums)
{
  for(int i = 0; i < 64; i++)
    nums[i] |= (col<<(i%8))^(row<<(i/8)*8);
}

void print64(uint64 num)
{
  int count = 0;
  for (uint64 p = BIT(63); p; p>>=1){
    cout << ((bool)(p & num) ? "@" : ".");
    if(++count %8 == 0)
      cout << endl;
  }
  cout << endl;
}

void print64_2(uint64 num, int i, int j)
{
  int count = 0;
  uint64 bi = BIT(i), bj = BIT(j);
  try{
    if(bi & num || bj & num)
      throw string("Invalid Connection");
  }catch(const string& e){
    cerr << e << endl;
    print64(bi);
    print64(bj);
    print64(num);
    exit(1);
  }
  for (uint64 p = BIT(63); p; p>>=1){
    if(p & bj) cout << 'j';
    else if (p & bi) cout << 'i';
    else cout << (p & num ? '@' : '.');
    if(++count %8 == 0)
      cout << endl;
  }
  cout << endl;
}

void saveImages(vector<uint64>& nums)
{
  int count = 64;
  sf::Image image;
  image.create(64,64,sf::Color::Black);
  for(auto& n: nums){
    count--;
    for(int i = 0; i < 64; i++)
      image.setPixel(i % 8 + (count % 8) * 8, i / 8 + (count / 8) * 8,
                    (uint64)0x1 << i & n ? sf::Color::Blue : sf::Color::White);
  }
  image.saveToFile("nums.png");
}

void knight(vector<uint64>& nums)
{
  for(int i = 0; i < 64; i++){
    nums[i] |= i < 20 ? (uint64)0xA1100110A<<i>>18 : (uint64)0xA1100110A<<(i-18);
    if(i % 8 < 3)
      nums[i] ^= nums[i]&_left;
    else if(i % 8 > 5)
      nums[i] ^= nums[i]&_right;
  }
}

void king(vector<uint64>& nums)
{
  for(int i = 0; i < 64; i++){
    nums[i] |= (i > 9 ? 0x70507ull << (i - 9) : 0x70507ull >> (9 - i));
    if(i % 8 == 0)nums[i] ^= nums[i] & _left;
    if(i % 8 == 7)nums[i] ^= nums[i] & _right;
  }
}

void connections(
  vector<vector<uint64>>& connections,
  const vector<uint64>& rects,
  const vector<uint64>& diags)
{
  uint64 temp = 0x0;
  for(int i = 0; i < 64; i++){
    for(int j = 0; j < 64; j++){
      if(rects[j] & BIT(i)){
        if(abs(i-j) < 8 && abs(i-j) > 1) // Pieces on same rank
          connections[i][j] = i < j ? (BIT(j) - BIT(i))^BIT(i) : (BIT(i) - BIT(j))^BIT(j);
        else if (j > i && j - i > 8){
          temp = BIT(j) >> 8;
          while(!(temp & BIT(i)))
            temp |= temp >> 8;
          connections[i][j] = temp ^ (BIT(i));
        }
        else if (j < i && i - j > 8){
          temp = BIT(j) << 8;
          while(!(temp & BIT(i)))
            temp |= temp << 8;
          connections[i][j] = temp ^ (BIT(i));
        }
      }
      else if(diags[j] & BIT(i)){
        if(j / 8 > i / 8){
          if(j % 8 > i % 8 && j - i > 9){ // j above left of i
            temp = BIT(j) >> 9;
            while(!(temp & BIT(i)))
              temp |= temp >> 9;
            connections[i][j] = temp ^ (BIT(i));
          }
          else if(j % 8 < i % 8 && j - i > 9){ // j above right of i
            temp = BIT(j) >> 7;
            while(!(temp & BIT(i)))
              temp |= temp >> 7;
            connections[i][j] = temp ^ (BIT(i));
          }
        }
        else{
          if(j % 8 > i % 8 && i - j > 9){ // j below left of i
            temp = BIT(j) << 7;
            while(!(temp & BIT(i)))
              temp |= temp << 7;
            connections[i][j] = temp ^ (BIT(i));
          }
          else if(j % 8 < i % 8 && i - j > 9){ // j below right of i
            temp = BIT(j) << 9;
            while(!(temp & BIT(i)))
              temp |= temp << 9;
            connections[i][j] = temp ^ (BIT(i));
          }
        }
      }
    }
  }
}

/*
int main(int argc, char* argv[])
{
  vector<uint64> nums  = vector<uint64>(64, 0x0ull);
  vector<uint64> nums2 = vector<uint64>(64, 0x0ull);
  vector<vector<uint64>> connect = vector<vector<uint64>>(64, vector<uint64>(64, 0x0));
  bool writeConnections = false;
  for(int i = 1; i < argc; i++){
    if(string(argv[i]) == "b")
      diags(nums);
    else if(string(argv[i]) == "r")
      rects(nums);
    else if(string(argv[i]) == "k")
      king(nums);
    else if(string(argv[i]) == "q"){
      diags(nums);
      rects(nums);
    }
    else if(string(argv[i]) == "n")
      knight(nums);
    else if(string(argv[i]) == "c"){
      diags(nums);
      rects(nums2);
      connections(connect, nums2, nums);
      writeConnections = true;
    }
  }

  //for(int i = 0; i < 64; i++){
  //  for(int j = 0; j < 64; j++){
  //    print64_2(connect[i][j],i,j);
  //  }
  //}

  // Print Boards to console and create png Image
  for(int i = 1; i < argc; i++){
    if(string(argv[i]) == "printBoard"){
      for(int j = 0; j < 64; j++){
        cout << "0x" << hex << nums[j] << endl;
        print64(nums[j]);
      }
      saveImages(nums);
      return 0;
    }
  }

  // Write images to File
  ofstream outFile("Connections.txt", ios::out);
  if(writeConnections){
    if(!outFile.is_open()){
      cerr << "Cannot create outfile! Out of disk space ?\n";
      return 1;
    }
    outFile << "static u64 CONNECTIONS[64][64] = {\n";
    for(auto& r : connect){
      outFile << "{\n";
      for(auto& e : r)
        outFile << "0x" << hex << e << ",\n";
      outFile << "},\n";
    }
  }
  else{
    for(auto& n : nums)
      outFile << "0x" << hex << n << ',' << endl;
    outFile.close();
  }

  return 0;
}
*/