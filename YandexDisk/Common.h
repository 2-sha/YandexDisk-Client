#pragma once
#include <string>
#include <vector>
using namespace std;

extern bool isTrash;
extern string currentDir, dataRef, configRef, clientId, clientSecret, token, refreshToken;

// ������� ������ � ����� ������� �������
string convertBytes(double bytes);

// 
void authorize(YandexDisk &yd, string &token, string &refreshToken);

// �������� ������ �� ������������
string getStr(bool global = false);

// �������� ������ ������ �� ����������
vector<vector<string>> getFileList(const char *root);

// ��������� ������ �� �����������
vector<string> split(string str, string sep);

// ������� ���� �� �������������� � ����������
void pathTuning(string &path);

// 
bool findInInput(string input, string command);

// �������� ������������ (Y/N):
bool getUserAnsw(string str);

// ������� ���������
int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);