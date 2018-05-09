#pragma once
#include "stdafx.h"

extern bool isTrash;
extern std::string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

// Перевод байтов в более крупные еденицы
std::string convertBytes(double bytes);

// 
// void authorize(YandexDisk &yd, string &token, string &refreshToken);

// Получить строку от пользователя
std::string getStr(bool global = false);

// Получить список файлов из директории
std::vector<std::vector<std::string>> getFileList(const char *root);

// Разделить строку по разделителю
std::vector<std::string> split(std::string str, std::string sep);

// Перевод путя из относительного в абсолютный
void pathTuning(std::string &path);

// 
bool findInInput(std::string input, std::string command);

// Спросить пользователя (Y/N):
bool getUserAnsw(std::string str);

// Функция прогресса
int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);