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
#include "YandexDisk.h"
using namespace std;

std::map<std::string, void(*)(std::string)> functions;

int main(int argc, char* argv[])
{
	setlocale(0, "ru");

	functions["help"] = &help;

	yandedisk::YandexDisk yd("6a7e79466d054c21b187f6dcb8d83e59", "bcafaddc48db434cab0124524e48f64b");

	ShellExecute(NULL, "open", "https://oauth.yandex.ru/authorize?response_type=code&client_id=6a7e79466d054c21b187f6dcb8d83e59", NULL, NULL, SW_SHOW);

	string input;
	cout << "Введите код подтверждения: ";
	cin >> input;
	yandedisk::Token data;
	try
	{
		data = yd.authorizationByCode(input);
	}
	catch (yandedisk::YandexDiskException ex)
	{
		std::cout << ex.what() << std::endl;
		std::cout << ex.getError() << std::endl;
	}

	cout << data.token << endl;
	cout << data.refreshToken << endl;

	system("pause");
	return 0;

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
		{
			size_t last = input.find_last_not_of(' ');
			input = input.substr(first, (last - first + 1));
		}
		// Удаление лишних пробелов между словами
		input.erase(
			unique(input.begin(), input.end(),
				[](const char &a, const char &b) { return &a == &b || (a == ' ' && b == ' '); }),
			input.end());
		// Замена \ на /
		replace(input.begin(), input.end(), '\\', '/');

		if (input.empty())
			continue;

		string command = split(input, " ")[0];
		if (functions.find(command) != functions.end())
		{
			(*functions[command])(input);
		}
		else
		{
			std::cout << "Команда " << command << " не найдена" << endl;
		}
	}
}