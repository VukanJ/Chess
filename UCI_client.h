#ifndef UCI_CLIENT_H
#define UCI_CLIENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <boost/tokenizer.hpp>

#include "misc.h"
#include "AI.h"

using namespace std;

#define ENGINE_NAME    "Engine_Name"
#define ENGINE_VERSION "0.1 Alpha"

class UCIclient
{
public:
	UCIclient();
	~UCIclient();
	void UCI_IO_loop();

	AI ai;
private:
	void uciNewGame();
	void printEngineID() const;
	void parsePosition(vector<string>&);
	void go(vector<string>&);
	enum class hostCommandCode {
		uci = 1, debug, isready, setoption, registerEngine,
		ucinewgame, position, go, stop, ponderhit, quit
	};
	enum class customOptions { 
		hash // Hash size in bytes 
	};
	ofstream outFile;
	map<string, customOptions>   getOptions;
	map<string, hostCommandCode> getHostCommandCode;
};


#endif