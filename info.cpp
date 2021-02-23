#include "info.h"

void sayHello()
{
	std::cout << std::string(53, '~') << '\n';
	std::cout << std::left;
	std::cout << std::setw(52) << "| \"Unnamed Chess Engine\" Version " << VERSION << "|\n";
	std::cout << std::setw(52) << "| Author: VukanJ -- https://github.com/VukanJ" << "|\n";
	std::cout << std::string(53, '~') << '\n';
}

void showLicense()
{
    std::ifstream licenseFile("License.txt", std::ios::in);
    if (!licenseFile.is_open()) {
        std::cout << "License file missing!.\n";
        std::cout << "Visit https://github.com/VukanJ/Chess for the full License\n";
        return;
    }
    std::string licenseText((std::istreambuf_iterator<char>(licenseFile)),
                             std::istreambuf_iterator<char>());
	licenseFile.close();
    std::cout << licenseText << '\n';
}

void showCredits()
{
	//cout << string(10,'~') << "CREDITS" << string(10,'~');
    std::cout << std::setfill('-') << std::left;
    std::cout << std::setw(30) << "Author"  << "Vukan Jevtic (github.com/VukanJ)\n";
}
