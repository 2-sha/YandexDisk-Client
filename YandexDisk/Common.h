#pragma once
#include <string>
#include <vector>
using namespace std;

extern bool isTrash;
extern string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

// Перевод байтов в более крупные еденицы
string convertBytes(double bytes);

// 
void authorize(YandexDisk &yd, string &token, string &refreshToken);

// Получить строку от пользователя
string getStr(bool global = false);

// Получить список файлов из директории
vector<vector<string>> getFileList(const char *root);

// Разделить строку по разделителю
vector<string> split(string str, string sep);

// Перевод путя из относительного в абсолютный
void pathTuning(string &path);

// 
bool findInInput(string input, string command);

// Спросить пользователя (Y/N):
bool getUserAnsw(string str);

// Функция прогресса
int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);