#pragma once
#include "stdafx.h"

extern bool isTrash;
extern std::string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

// ������� ������ � ����� ������� �������
std::string convertBytes(double bytes);

// 
// void authorize(YandexDisk &yd, string &token, string &refreshToken);

// �������� ������ �� ������������
std::string getStr(bool global = false);

// �������� ������ ������ �� ����������
std::vector<std::vector<std::string>> getFileList(const char *root);

// ��������� ������ �� �����������
std::vector<std::string> split(std::string str, std::string sep);

// ������� ���� �� �������������� � ����������
void pathTuning(std::string &path);

// 
bool findInInput(std::string input, std::string command);

// �������� ������������ (Y/N):
bool getUserAnsw(std::string str);

// ������� ���������
int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);