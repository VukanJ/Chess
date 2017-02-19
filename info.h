#ifndef INFO_H
#define INFO_H

#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>

using namespace std;

const string VERSION = "0.1-alpha";

void sayHello()
{
	cout << string(53, '~') << endl;
	cout << left;
	cout << setw(52) << string("| \"Unnamed Chess Engine\" Version " + VERSION) << "|\n";
	cout << setw(52) << "| Author: VukanJ -- https://github.com/VukanJ" << "|\n";
	cout << setw(52) << "| Press <C> for credits" << "|\n";
	cout << setw(52) << "| Press <L> for license" << "|\n";
	cout << string(53, '~') << endl;
}

void showLicense()
{
	ifstream licenseFile("License.txt", ios::in);
	if(!licenseFile.is_open()){
		cout << "License file missing!.\n";
		cout << "Visit https://github.com/VukanJ/Chess for the full License\n";
		return;
	}
	string licenseText((istreambuf_iterator<char>(licenseFile)),
										 istreambuf_iterator<char>());
	licenseFile.close();
	cout << licenseText << endl;
}

void showCredits()
{
	//cout << string(10,'~') << "CREDITS" << string(10,'~');
	cout << setfill('-') << left;
	cout << setw(30) << "Author"  << "VukanJ  (github.com/VukanJ)\n";
	cout << setw(30) << "Fonts"   << "Mozilla (github.com/mozilla)\n";
	cout << setw(30) << "Sprites" << "jurgenwesterhof\n";
	cout << setw(30) << "Sprite Source" << "wikimedia.org/wiki/Template:SVG_chess_pieces\n";
	cout << setw(30) << "Code" << "C++ && SFML && C++Boost\n";
}

#endif
