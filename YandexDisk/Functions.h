#pragma once

#include "Common.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;

map<string, void(*)()> functions;

void help()