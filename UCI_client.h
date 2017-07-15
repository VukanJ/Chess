#ifndef UCI_CLIENT_H
#define UCI_CLIENT_H

#include <iostream>
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
	void UCI_IO_loop();

	AI ai;
private:
	void startNewGame();
	void printEngineID() const;
	enum class hostCommandCode {
		uci = 1, debug, isready, setoption, registerEngine,
		ucinewgame, position, go, stop, ponderhit, quit
	};
	enum class customOptions { 
		hash // Hash size in bytes 
	};
	map<string, customOptions>   getOptions;
	map<string, hostCommandCode> getHostCommandCode;
};


#endif