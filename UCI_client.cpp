#include "UCI_client.h"

UCIclient::UCIclient() : ai(AI("*", black))
{
	// Standard computer opponent is black
	// Starting from standard position
	getHostCommandCode = {
		{ "uci",        hostCommandCode::uci },
		{ "debug",      hostCommandCode::debug },
		{ "isready",    hostCommandCode::isready },
		{ "setoption",  hostCommandCode::setoption },
		{ "register",   hostCommandCode::registerEngine },
		{ "ucinewgame", hostCommandCode::ucinewgame },
		{ "position",   hostCommandCode::position },
		{ "go",         hostCommandCode::go },
		{ "stop",       hostCommandCode::stop },
		{ "ponderhit",  hostCommandCode::ponderhit },
		{ "quit",       hostCommandCode::quit }
	};
	getGoArgCode = {
		{ "depth",    goArgument::depth },
		{ "infinite", goArgument::infinite },
		{ "mate",     goArgument::mate },
		{ "movetime", goArgument::movetime }
	};
}

void UCIclient::UCI_IO_loop()
{
	// Communicates with external GUI (Host)
	string input;
	vector<string> inputList;
	while (true) {
		
		waitForInput(inputList);
		interpretInput(inputList);
		
	}
}

void UCIclient::waitForInput(vector<string>& input)
{
	input.clear();
	string line;
	getline(cin, line); // User or GUI input
					    // Tokenize input
	boost::trim(line);
	boost::escaped_list_separator<char> sep("", " ", "");
	boost::tokenizer<boost::escaped_list_separator<char>> tokenize(line, sep);
	outFile.open("turnierLog.log", ios::app);
	for (auto token : tokenize) {
		input.push_back(token);

		//cout << token << ' ';
		outFile << token << ' ';
	}
	outFile << '\n';
	outFile.close();
}

void UCIclient::interpretInput(vector<string>& inputList)
{
	if (inputList.empty()) return;
	switch (getHostCommandCode[inputList[0]]) {
	case hostCommandCode::uci:
		printEngineID();
		break;
	case hostCommandCode::debug:
		break;
	case hostCommandCode::isready:
		cout << "readyok\n";
		break;
	case hostCommandCode::setoption:
		break;
	case hostCommandCode::registerEngine:
		/* Engine does not require registering */
		break;
	case hostCommandCode::ucinewgame:
		uciNewGame();
		cout << "readyok\n";
		break;
	case hostCommandCode::position:
		parsePosition(inputList);
		break;
	case hostCommandCode::go:
		go(inputList);
		break;
	case hostCommandCode::stop:
		break;
	case hostCommandCode::ponderhit:
		/* Engine doesnt care */
		break;
	case hostCommandCode::quit:
		return;
	default:
		cout << "Unknown command: " << inputList[0] << '\n';
	}
}

void UCIclient::printEngineID() const
{
	printf("id name %s %s\n", ENGINE_NAME, ENGINE_VERSION);
	cout << "id author Vukan Jevtic (github.com/VukanJ)\n"
		    "option name Hash type spin\n"
		    "uciok\n";
}

void UCIclient::uciNewGame()
{
	// Resets hash tables and sets position to standard starting position
	ai.reset();
	ai.resetHash();
	ai.currentAge = 0;
}

void UCIclient::parsePosition(vector<string>& inputList)
{
	inputList.erase(inputList.begin()); // "position"
	if (inputList.empty()) {
		return;
	}
	if (*inputList.begin() == "startpos") {
		inputList.erase(inputList.begin()); // "startpos"
		ai.sideToMove = white;
		// Standard starting position
		ai.setFen("*");
	}
	else if (*inputList.begin() == "fen") {
		// Custom initial position
		inputList.erase(inputList.begin()); // "fen"
		if (inputList.size() < 6) {
			cerr << "Invalid fen!\n";
			return;
		}
		string FEN;
		ai.sideToMove = (inputList[1] == "w" ? white : black);
		for (int i = 0; i < 6; ++i) 
			FEN += inputList[i] + ' ';
		inputList.erase(inputList.begin(), inputList.begin() + 6);
		ai.setFen(FEN);
		//ai.printAscii();
	}
	if (inputList.empty()) { 
		// ready
		return; 
	}
	else if (inputList[0] == "moves") {
		// Play given moves
		inputList.erase(inputList.begin()); // "moves"
		ai.playStringMoves(inputList, ai.sideToMove);
	}
	//ai.printAscii();
}

void UCIclient::go(vector<string>& inputList)
{
	pair<Move, Move> bestMoves;
	inputList.erase(inputList.begin());
	if (inputList.empty()) {
		bestMoves = ai.getBestMove(ai.sideToMove, 5, true);
	}
	else {
		switch (getGoArgCode[*inputList.begin()]) {
		case goArgument::depth:
			inputList.erase(inputList.begin()); // "depth"
			if (inputList.empty()) {
				cerr << "No depth specified!\n";
				return;
			}
			bestMoves = ai.getBestMove(ai.sideToMove, stoi(inputList[0]), true);
			break;
		case goArgument::infinite:
			// Create thread, run until "stop" is passed
			break;
		case goArgument::movetime:
			// Run timed NegaMax for x seconds
			break;
		case goArgument::mate:
			// Search mate in x, same as search to specified depth
			inputList.erase(inputList.begin()); // "mate"
			if (inputList.empty()) {
				cerr << "# Moves to mate not specified!\n";
				return;
			}
			bestMoves = ai.getBestMove(ai.sideToMove, stoi(inputList[0]), true);
			break;
		}
	}
	cout << "bestmove " << shortNotation(bestMoves.first)
		 << " ponder " << shortNotation(bestMoves.second) << '\n';
	outFile.open("turnierLog.log", ios::app);
	outFile << "best = " << shortNotation(bestMoves.first) << '\n';
	outFile.close();
	//ai.currentAge++;
}
