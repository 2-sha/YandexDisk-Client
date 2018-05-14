#include "Common.h"

bool isTrash;
std::string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

std::vector<std::string> split(std::string str, std::string sep) {
	std::vector<std::string> parts;
	int index = 0, prevIndex = 0, l = sep.size();
	index = str.find(sep, index);
	if (index == std::string::npos)
	{
		parts.push_back(str);
		return parts;
	}
	parts.push_back(str.substr(0, index));
	while (index != std::string::npos) {
		prevIndex = index;
		index = str.find(sep, index + 1);
		parts.push_back(str.substr(prevIndex + l, index - prevIndex - l));
	}
	return parts;
}