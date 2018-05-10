#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>

extern bool isTrash;
extern std::string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

extern std::map<std::string, void(*)(std::string)> functions;