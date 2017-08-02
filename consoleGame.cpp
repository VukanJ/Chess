#include "consoleGame.h"

ConsoleGame::ConsoleGame() : ai(AI("*", black))
{
	mapToCode = map<string,userCode>{ 
		{ "newgame", userCode::newgame },
		{ "setposition", userCode::setposition },
		{ "help", userCode::help}
	};
	ai.printAscii();
}

void ConsoleGame::ioLoop(string initialCommand)
{
	vector<string> inputList;
	boost::trim(initialCommand);
	boost::escaped_list_separator<char> sep("", " ", "");
	boost::tokenizer<boost::escaped_list_separator<char>> tokenize(initialCommand, sep);
	for (auto token : tokenize) {
		inputList.push_back(token);
	}
	interpretInput(inputList);
	while (true) {

		waitForInput(inputList);
		interpretInput(inputList);

	}
}

void ConsoleGame::waitForInput(vector<string>& inputList) 
{
	string userInput;
	inputList.clear();
	getline(cin, userInput);
	// Tokenize input
	boost::trim(userInput);
	boost::escaped_list_separator<char> sep("", " ", "");
	boost::tokenizer<boost::escaped_list_separator<char>> tokenize(userInput, sep);
	for (auto token : tokenize) {
		inputList.push_back(token);
	}
}

void ConsoleGame::interpretInput(const vector<string>& inputList)
{
	if (inputList.empty()) return;
	switch (mapToCode[inputList[0]]) {
	case userCode::newgame: break;
	case userCode::setposition: break;
	case userCode::help: cout << inGameHelp << endl; break;
	}
}