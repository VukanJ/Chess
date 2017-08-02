#ifndef CONSOLE_GAME
#define CONSOLE_GAME

#include <iostream>
#include <boost/tokenizer.hpp>
#include <map>

#include "AI.h"

using namespace std;

// Game loop inside console without uci

class ConsoleGame
{
public:
	ConsoleGame();
	void ioLoop(string initialCommand);
private:
	enum class userCode{
		newgame, // [white, w, black, b] start new game		
		setposition, // [FEN] set position according to fen	
		help, // show help
	};
	void waitForInput(vector<string>& inputList);
	void interpretInput(const vector<string>& inputList);
	map<string, userCode> mapToCode;
	AI ai;
};

const string inGameHelp = 
	"Help:\n"
	"\t\'startnewgame <b/w>\': Start a new game as black / white\n"
	"\t\'setposition <FEN>\': Set the board position according to FEN string\n"
	"\tMoves are parsed by defining from and to square. Example:\n"	
	"\t\t \'e2e3\' = Move piece from e2 to e3\n"
	"\t\t \'e7e8q\' = Promote pawn to queen\n"
	"\t\t \'e1g1\' = Castle kingside\n"
	"\t\t Specifier \'x\' is not required for parsing captures\n"
	"\t\'help\': Show this info";


#endif