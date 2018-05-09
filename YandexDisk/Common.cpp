#include "stdafx.h"

std::string getStr(bool global) {
	std::string input;
	if (global) {
		if (::isTrash)
			std::cout << "trash:" << ::currentDir << ">";
		else
			std::cout << "disk:" << ::currentDir << ">";
	}
	getline(std::cin, input, '\n');
	// Удаление пробелов с начала и конца
	size_t first = input.find_first_not_of(' ');
	if (std::string::npos == first)
		return input;
	size_t last = input.find_last_not_of(' ');
	input = input.substr(first, (last - first + 1));
	// Удаление лишних пробелов между словами
	input.erase(
		unique(input.begin(), input.end(),
			[](const char &a, const char &b) { return &a == &b || (a == ' ' && b == ' '); }),
		input.end());
	// Замена \ на /
	replace(input.begin(), input.end(), '\\', '/');
	return input;
}

bool getUserAnsw(std::string str) {
	std::string input;
	while (true) {
		std::cout << str << " (Y/N): ";
		input = getStr();
		if (findInInput(input, "y"))
			return true;
		else if (findInInput(input, "n"))
			return false;
		else
			std::cout << "Ошибка: Некорректное значение" << std::endl;
	}
}

void pathTuning(std::string &path) {
	size_t pos = path.find("/.."), last_of;
	while (pos != std::string::npos) {
		if (pos == 0) {
			path.erase(0, 3);
		}
		else {
			last_of = path.find_last_of("/", pos - 1);
			path.erase(last_of, pos - last_of + 3);
		}
		pos = path.find("/..");
	}
	if (path.empty())
		path = "/";
}

std::vector<std::string> split(std::string str, std::string sep) {
	std::vector<std::string> parts;
	int index = 0, prevIndex = 0, l = sep.size();
	index = str.find(sep, index);
	if (index == std::string::npos)
		return parts;
	parts.push_back(str.substr(0, index));
	while (index != std::string::npos) {
		prevIndex = index;
		index = str.find(sep, index + 1);
		parts.push_back(str.substr(prevIndex + l, index - prevIndex - l));
	}
	return parts;
}
