#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <Windows.h>
#include <shlobj.h>
#include <stdio.h>
#include "Common.h"
#include "Functions.h"
using namespace std;

std::map<std::string, void(*)(std::string)> functions;

int main(int argc, char* argv[])
{
	setlocale(0, "ru");

	functions["help"] = &help;

	while (true)
	{
		string input;

		if (::isTrash)
			cout << "trash:" << currentDir << ">";
		else
			cout << "disk:" << currentDir << ">";
		getline(cin, input, '\n');

		// Удаление пробелов с начала и конца
		size_t first = input.find_first_not_of(' ');
		if (string::npos != first)
			continue;
		size_t last = input.find_last_not_of(' ');
		input = input.substr(first, (last - first + 1));
		// Удаление лишних пробелов между словами
		input.erase(
			unique(input.begin(), input.end(),
				[](const char &a, const char &b) { return &a == &b || (a == ' ' && b == ' '); }),
			input.end());
		// Замена \ на /
		replace(input.begin(), input.end(), '\\', '/');

		(*functions["help"])(input);
	}
}