#pragma comment(lib, "curllib.lib")
#pragma comment(lib, "Shell32.lib")
#include "YandexDisk.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <shlobj.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <iterator>
#include <time.h>
#include <iomanip>
using namespace std;
using namespace yandexdisk;

bool flag, isTrash;
string input, currentDir = "/", dataRef, configRef, clientId, clientSecret, token, refreshToken;
vector<File> fileList;
const int helpWidth = 22;

// ��������� �������
string convertBytes(double bytes);
void authorize(YandexDisk &yd, string &token, string &refreshToken);
string getStr(bool global = false);
vector<vector<string>> getFileList(const char *root);
vector<string> split(string str, string sep);
void pathTuning(string &path);
bool findInInput(string input, string command);
bool getUserAnsw(string str);
int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow);

int main(int argc, char* argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout.setf(ios::left);

	size_t sz;
	char* buf = nullptr;
	_dupenv_s(&buf, &sz, "homepath");
	dataRef = buf;
	dataRef = "C:" + dataRef + "\\Documents\\YandexDiskClient";
	configRef = dataRef + "\\common.ini";

	ifstream file(configRef);
	if (!file.is_open()) {
		CreateDirectory(dataRef.c_str(), NULL);
		ofstream file(configRef);
		file << "[common]" << endl;
		file << "client_id=6a7e79466d054c21b187f6dcb8d83e59" << endl;
		file << "client_secret=bcafaddc48db434cab0124524e48f64b" << endl;
	}
	file.close();

	char str[255];
	GetPrivateProfileString("common", "client_id", NULL, str, sizeof(str), configRef.c_str());
	clientId = str; // � ������� ������ �������� string
	GetPrivateProfileString("common", "client_secret", NULL, str, sizeof(str), configRef.c_str());
	clientSecret = str;
	GetPrivateProfileString("common", "access_token", NULL, str, sizeof(str), configRef.c_str());
	token = str;
	GetPrivateProfileString("common", "refresh_token", NULL, str, sizeof(str), configRef.c_str());
	refreshToken = str;

	YandexDisk yandexDisk(clientId, clientSecret);

	if (token.empty())
		authorize(yandexDisk, token, refreshToken);

	yandexDisk.setToken(token);
	yandexDisk.setProgressFunc(progress);

	while (true) {
		input = getStr(true);

		// ����� �� ����������
		if (findInInput(input, "exit"))
			return 0;

		// ����� �������
		if (findInInput(input, "help")) {
			cout << setw(helpWidth) << "exit" << "����� �� ����������" << endl;
			cout << setw(helpWidth) << "help" << "������ ������" << endl;
			cout << setw(helpWidth) << "space info" << "����� ������ �� �����" << endl;
			cout << setw(helpWidth) << "logout" << "����� �� ������� ������" << endl;
			cout << setw(helpWidth) << "cd %path%" << "������� ����������" << endl;
			cout << setw(helpWidth) << "ls" << "������ ������ � ����������" << endl;
			cout << setw(helpWidth) << "open %path%" << "������� ����" << endl;
			cout << setw(helpWidth) << "clear cache" << "�������� ���" << endl;
			cout << setw(helpWidth) << "download %from% %to%" << "��������� ������" << endl;
			cout << setw(helpWidth) << "mkdir %path%" << "������� �����" << endl;
			cout << setw(helpWidth) << "delete %path%" << "������� ������" << endl;
			cout << setw(helpWidth) << "config" << "������� ����� � �����������" << endl;
			cout << setw(helpWidth) << "upload %from% %to%" << "��������� ���� � ���������� �� ����" << endl;
			cout << setw(helpWidth) << "upload %url% %to%" << "��������� ���� �� ��������� �� ����" << endl;
			cout << setw(helpWidth) << "disk" << "������������� � ����" << endl;
			cout << setw(helpWidth) << "trash" << "������������� � �������" << endl;
			cout << setw(helpWidth) << "recover %name%" << "������������ ���� �� ������� ��� ������� ���������" << endl;
			cout << setw(helpWidth) << "recover %name% &to%" << "������������ ���� �� ������� ��� ����� ���������" << endl;
		}

		// ������� ����� � ����������� � �����
		else if (findInInput(input, "config")) {
			ShellExecute(0, "open", dataRef.c_str(), NULL, NULL, SW_SHOW);
		}

		// ������ � ��������� ������ �� �����
		else if (findInInput(input, "space info")) {
			// ���� � 1 ������� �� ��������� ������, �� ������ ����� � ��������� ���� �� ���������
			float data = yandexDisk.getDiskData("total_space");
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				data = yandexDisk.getDiskData("total_space");
			}

			cout << "�����: " << convertBytes(data) << endl;
			cout << "������: " << convertBytes(yandexDisk.getDiskData("used_space")) << endl;
			cout << "� �������: " << convertBytes(yandexDisk.getDiskData("trash_size")) << endl;
		}

		// ����� �� ������� ������ �������
		else if (findInInput(input, "logout")) {
			if (!getUserAnsw("�� �������?"))
				continue;
			WritePrivateProfileString("common", "access_token", NULL, configRef.c_str());
			WritePrivateProfileString("common", "refresh_token", NULL, configRef.c_str());
			yandexDisk.deleteToken();
			token = "";
			refreshToken = "";
			cout << "����� ��������!" << endl;
		}

		// ������ ������ � ������� ����������
		else if (findInInput(input, "ls")) {
			if (isTrash)
				fileList = yandexDisk.getTrashList(currentDir);
			else
				fileList = yandexDisk.getFileList(currentDir);

			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				if (isTrash)
					fileList = yandexDisk.getTrashList(currentDir);
				else
					fileList = yandexDisk.getFileList(currentDir);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ���������� \"" << currentDir << "\" ������ �� ����������" << endl;
				continue;
			}

			if (fileList.empty()) {
				cout << "� ������ ����� ��� ������" << endl;
				continue;
			}

			cout << setw(7) << "���";
			cout << setw(12) << "������";
			cout << setw(20) << "������";
			cout << setw(20) << "������";
			cout << "��������" << endl;

			for (size_t i = 0; i < fileList.size(); i++) {
				char b[80];

				cout << setw(7) << fileList[i].type; // ���

				cout << setw(12) << ((!fileList[i].size) ? "" : convertBytes(static_cast<double>(fileList[i].size))); // ������

				strftime(b, 80, "%d %b %Y %H:%M", &fileList[i].created);
				cout << setw(20) << b; // ���� ��������

				strftime(b, 80, "%d %b %Y %H:%M", &fileList[i].modified);
				cout << setw(20) << b; // ���� ���������

				cout << fileList[i].name; // ��������

				cout << endl;
			}
		}

		// ������� ����������
		else if (findInInput(input, "cd")) {
			if (isTrash) {
				cout << "������: � ������� ������ ������������ �� ������" << endl;
				continue;
			}

			input.erase(0, 3);

			if (!input.size()) {
				cout << "������: �� �� ����� ����� ����������" << endl;
				continue;
			}

			if (input.substr(0, 1) == "\"")
				input = split(input, "\"")[1];

			if (input.substr(0, 1) != "/")
				input = currentDir + ((currentDir.substr(currentDir.size() - 1, currentDir.size()) != "/") ? "/" : "") + input;

			pathTuning(input);
			File data = yandexDisk.getMetaInfo(input);

			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				data = yandexDisk.getMetaInfo(input);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}

			currentDir = input;
		}

		// ������������� � �������
		else if (findInInput(input, "trash")) {
			if (isTrash) {
				cout << "������: �� ��� � �������" << endl;
				continue;
			}

			isTrash = true;
			currentDir = "/";
		}

		// ������������� � ����
		else if (findInInput(input, "disk")) {
			if (!isTrash) {
				cout << "������: �� ��� � �����" << endl;
				continue;
			}

			isTrash = false;
			currentDir = "/";
		}

		// ������� ����
		else if (findInInput(input, "open")) {
			if (isTrash) {
				cout << "������: ������ ������� ���� �� �������" << endl;
				continue;
			}

			input.erase(0, 5);

			if (!input.size()) {
				cout << "������: �� �� ����� ����� ������������ �����" << endl;
				continue;
			}

			if (input.substr(0, 1) == "\"")
				input = split(input, "\"")[1];

			string path;
			if (input.substr(0, 1) != "/") {
				path = dataRef + "\\" + input;
				input = currentDir + ((currentDir == "/") ? "" : "/") + input;
			}
			else
				path = dataRef + "\\" + input.substr(input.find_last_of("\\") + 1);
			pathTuning(input);

			string type = yandexDisk.getMetaInfo(input).type;
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				type = yandexDisk.getMetaInfo(input).type;
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (type == "dir") {
				cout << "������: ����������� ����� ������ �����" << endl;
				cout << "��� �������� � ���������� �������������� �������� cd ���_����������" << endl;
				continue;
			}

			ifstream file(path);
			if (file.is_open()) {
				file.close();
				cout << "���� ��������� ������ ���� ������������ ��������� �� ��������� ��� ����� ����" << endl;
				ShellExecute(0, "open", path.c_str(), NULL, NULL, SW_SHOW);
				continue;
			}

			yandexDisk.downloadFile(input, path);

			cout << "���� ��������� ������ ���� ������������ ��������� �� ��������� ��� ����� ����" << endl;

			ShellExecute(0, "open", path.c_str(), NULL, NULL, SW_SHOW);
			vector<vector<string>> list = getFileList([](string a, string b) { return a + b; }(dataRef, "\\*").c_str());

			if (list.size() >= 6) {
				int minTime = 2147483647;
				string minName = "";
				for (size_t i = 0; i < list.size(); i++) {
					if (atoi(list[i][2].c_str()) < minTime && list[i][0] != "common.ini") {
						minTime = atoi(list[i][2].c_str());
						minName = list[i][0];
					}
				}
				minName = dataRef + "\\" + minName;
				remove(minName.c_str());
			}
		}

		// �������� ���
		else if (findInInput(input, "clear cache")) {
			vector<vector<string>> list = getFileList([](string a, string b) { return a + b; }(dataRef, "\\*").c_str());

			for (size_t i = 0; i < list.size(); i++) {
				if (list[i][0] == "common.ini")
					continue;
				list[i][0] = dataRef + "\\" + list[i][0];
				remove(list[i][0].c_str());
			}

			cout << "������� ������: " << list.size() - 1 << endl;
		}

		// ������� ����
		else if (findInInput(input, "download")) {
			if (isTrash) {
				cout << "������: ������ ��������� ���� �� �������" << endl;
				continue;
			}

			input.erase(0, 9);

			if (split(input, " ").empty()) {
				cout << "������: ������� ���� ����������" << endl;
				continue;
			}

			string from, to;
			if (input.substr(0, 1) == "\"") {
				from = split(input, "\"")[1];
				input.erase(0, input.find("\"", 1) + 2);
			}
			else {
				from = split(input, " ")[0];
				input.erase(0, input.find(" ", 1) + 1);
			}
			if (input.substr(input.size() - 1, 1) == "\"")
				to = split(input, "\"")[1];
			else
				to = input;

			if (from.substr(0, 1) != "/")
				from = currentDir + ((currentDir == "/") ? "" : "/") + from;
			pathTuning(from);
			pathTuning(to);

			string type = yandexDisk.getMetaInfo(from).type;
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				type = yandexDisk.getMetaInfo(from).type;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������ ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (type == "dir") {
				cout << "������: ��������� ����� ������ �����" << endl;
				continue;
			}
			ifstream file(to);
			if (file.is_open() && !getUserAnsw("����� ���� ��� ����������. ������������?")) {
				file.close();
				continue;
			}

			yandexDisk.downloadFile(from, to);

			if (yandexDisk.getError() == "Unable to open file") {
				cout << "������: ���������� ����� ��������� ���� �� ���������� ������" << endl;
				continue;
			}

			cout << "���������" << endl;
		}

		// ������� ����������
		else if (findInInput(input, "mkdir")) {
			if (isTrash) {
				cout << "������: ������ ������� ���������� � �������" << endl;
				continue;
			}

			input.erase(0, 6);

			if (!input.size()) {
				cout << "������: �� �� ����� ����� ����������� ����������" << endl;
				continue;
			}

			if (input.substr(0, 1) == "\"")
				input = split(input, "\"")[1];

			if (input.substr(0, 1) != "/")
				input = currentDir + ((currentDir == "/") ? "" : "/") + input;

			pathTuning(input);
			yandexDisk.makeDir(input);
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				yandexDisk.makeDir(input);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskPathPointsToExistentDirectoryError") {
				cout << "������: ������ ����� ��� ����������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskPathDoesntExistsError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}

			cout << "����� �� ������ \"" << input << "\" �������" << endl;
		}

		// ������� ����� ��� ����
		else if (findInInput(input, "delete")) {
			input.erase(0, 7);

			if (!input.size()) {
				cout << "������: �� �� ����� ����� ���������� �����" << endl;
				continue;
			}

			if (input.substr(0, 1) == "\"")
				input = split(input, "\"")[1];

			if (!getUserAnsw("�� �������?"))
				continue;
			
			if (isTrash)
				yandexDisk.deleteTrash(input);
			else {
				if (input.substr(0, 1) != "/")
					input = currentDir + ((currentDir == "/") ? "" : "/") + input;

				pathTuning(input);

				yandexDisk.deleteResource(input);
			}

			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				if (isTrash)
					yandexDisk.deleteTrash(input);
				else
					yandexDisk.deleteResource(input);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "�������� ��������. ������� ����� ������ ��������������� �����" << endl;
			else
				cout << "�������!" << endl;
		}

		// ����������� ����� ��� ����
		else if (findInInput(input, "move")) {
			if (isTrash) {
				cout << "������: ��� �������������� ����� �� ������� �������������� �������� recover" << endl;
				continue;
			}

			input.erase(0, 5);

			if (split(input, " ").empty()) {
				cout << "������: ������� ���� ����������" << endl;
				continue;
			}

			string from, to;
			if (input.substr(0, 1) == "\"") {
				from = split(input, "\"")[1];
				input.erase(0, input.find("\"", 1) + 2);
			}
			else {
				from = split(input, " ")[0];
				input.erase(0, input.find(" ", 1) + 1);
			}
			if (input.substr(input.size() - 1, 1) == "\"")
				to = split(input, "\"")[1];
			else
				to = input;

			if (from.substr(0, 1) != "/")
				from = currentDir + ((currentDir == "/") ? "" : "/") + from;
			if (to.substr(0, 1) != "/")
				to = currentDir + ((currentDir == "/") ? "" : "/") + to;
			pathTuning(from);
			pathTuning(to);

			yandexDisk.moveResource(from, to, false);
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				yandexDisk.moveResource(from, to, false);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("����� ������ ��� ����������. ������������?"))
					continue;

				yandexDisk.moveResource(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "����������� ��������. ������� ����� ������ ��������������� �����" << endl;
			else
				cout << "������ ������� ���������" << endl;
		}

		// ����������� ����� ��� ����
		else if (findInInput(input, "copy")) {
			if (isTrash) {
				cout << "������: ��� �������������� ����� �� ������� �������������� �������� recover" << endl;
				continue;
			}

			input.erase(0, 5);

			if (split(input, " ").empty()) {
				cout << "������: ������� ���� ����������" << endl;
				continue;
			}

			string from, to;
			if (input.substr(0, 1) == "\"") {
				from = split(input, "\"")[1];
				input.erase(0, input.find("\"", 1) + 2);
			}
			else {
				from = split(input, " ")[0];
				input.erase(0, input.find(" ", 1) + 1);
			}

			if (input.substr(input.size() - 1, 1) == "\"")
				to = split(input, "\"")[1];
			else
				to = input;

			if (from.substr(0, 1) != "/")
				from = currentDir + ((currentDir == "/") ? "" : "/") + from;
			if (to.substr(0, 1) != "/")
				to = currentDir + ((currentDir == "/") ? "" : "/") + to;
			pathTuning(from);
			pathTuning(to);

			yandexDisk.copyResourse(from, to, false);
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				yandexDisk.copyResourse(from, to, false);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("����� ������ ��� ����������. ������������?"))
					continue;

				yandexDisk.copyResourse(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "����������� ��������. ������� ����� ������ ��������������� �����" << endl;
			else
				cout << "������ ������� ����������" << endl;
		}

		// ��������� ����
		else if (findInInput(input, "upload")) {
			if (isTrash) {
				cout << "������ ��������� ���� � �������" << endl;
				continue;
			}

			input.erase(0, 7);

			if (split(input, " ").empty()) {
				cout << "������: ������� ���� ����������" << endl;
				continue;
			}

			string from, to;
			if (input.substr(0, 1) == "\"") {
				from = split(input, "\"")[1];
				input.erase(0, input.find("\"", 1) + 2);
			}
			else {
				from = split(input, " ")[0];
				input.erase(0, input.find(" ", 1) + 1);
			}
			if (input.substr(input.size() - 1, 1) == "\"")
				to = split(input, "\"")[1];
			else
				to = input;

			if (to.substr(0, 1) != "/")
				to = currentDir + ((currentDir == "/") ? "" : "/") + to;
			pathTuning(to);

			if (from.substr(0, 4) == "http")
				yandexDisk.uploadFileFromUrl(from, to);
			else
				yandexDisk.uploadFile(from, to, false);

			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				if (from.substr(0, 4) == "http")
					yandexDisk.uploadFileFromUrl(from, to);
				else
					yandexDisk.uploadFile(from, to, false);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "Unable to open file") {
				cout << "������: ���������� ����� ��������� ���� �� ���������� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("����� ������ ��� ����������. ������������?"))
					continue;

				yandexDisk.uploadFile(from, to, true);
			}
			if (!yandexDisk.getOperationStatus().empty())
				cout << "������� ��������. ������� ����� ������ ��������������� �����" << endl;
			else
				cout << "���������" << endl;
		}

		// �������������� ����� �� �������
		else if (findInInput(input, "recover")) {
			if (!isTrash) {
				cout << "������: ������� recover ��������� ������ � �������" << endl;
				continue;
			}

			input.erase(0, 8);

			if (!input.size()) {
				cout << "������: �� �� ����� ����� ���������� �����" << endl;
				continue;
			}

			string from, to = "";
			if (input.find(" ") == string::npos)
				from = input;
			else {
				if (input.substr(0, 1) == "\"") {
					from = split(input, "\"")[1];
					input.erase(0, input.find("\"", 1) + 2);
				}
				else {
					from = split(input, " ")[0];
					input.erase(0, input.find(" ", 1) + 1);
				}
				if (!input.empty() && input.substr(input.size() - 1, 1) == "\"")
					to = split(input, "\"")[1];
				else
					to = input;
			}

			yandexDisk.recoverResoure(from, to, false);
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				yandexDisk.recoverResoure(from, to, false);
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "������: ��� ���������� � ��������. ���������� ��� ���" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "������: ��������� ���� �� ������" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("����� ������ ��� ����������. ������������?"))
					continue;

				yandexDisk.recoverResoure(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "�������������� ��������. ������� ����� ������ ��������������� �����" << endl;
			else
				cout << "������ ������� ������������" << endl;
		}

		else
			cout << "������: ����������� �������" << endl;
	}
}

int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow) {

	if (dltotal > 0) {
		if (dltotal == dlnow)
			printf("\r%15c\r", ' ');
		else
			cout << "\r��������: " << int(dlnow * 100 / dltotal) << "%";
	}

	else if (ultotal > 0) {
		if (ultotal == ulnow)
			printf("\r%15c\r", ' ');
		else
			cout << "\r��������: " << int(ulnow * 100 / ultotal) << "%";
	}

	return 0;
}

bool getUserAnsw(string str) {
	string input;
	while (true) {
		cout << str << " (Y/N): ";
		input = getStr();
		if (findInInput(input, "y"))
			return true;
		else if (findInInput(input, "n"))
			return false;
		else
			cout << "������: ������������ ��������" << endl;
	}
}

bool findInInput(string input, string command) {
	transform(input.begin(), input.end(), input.begin(), tolower);
	int pos = input.find(command);
	if (pos != string::npos && pos == 0)
		return true;
	return false;
}

void pathTuning(string &path) {
	size_t pos = path.find("/.."), last_of;
	while (pos != string::npos) {
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

vector<string> split(string str, string sep) {
	vector<string> parts;
	int index = 0, prevIndex = 0, l = sep.size();
	index = str.find(sep, index);
	if (index == string::npos)
		return parts;
	parts.push_back(str.substr(0, index));
	while (index != string::npos) {
		prevIndex = index;
		index = str.find(sep, index + 1);
		parts.push_back(str.substr(prevIndex + l, index - prevIndex - l));
	}
	return parts;
}

vector<vector<string>> getFileList(const char *root) {
	WIN32_FIND_DATA fd;
	vector<vector<string>> list;
	int counter = 0;

	HANDLE hFind = ::FindFirstFile(root, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		ULARGE_INTEGER ull;
		File file;
		do {
			ull.LowPart = fd.ftCreationTime.dwLowDateTime;
			ull.HighPart = fd.ftCreationTime.dwHighDateTime;
			list.resize(list.size() + 1);
			list[counter].resize(3);
			list[counter][0] = fd.cFileName;
			list[counter][1] = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "folder" : "file";
			list[counter][2] = to_string(ull.QuadPart / 10000000ULL - 11644473600ULL);
			counter++;
		}
		while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	list.erase(list.begin());
	list.erase(list.begin());

	return list;
}

string getStr(bool global) {
	string input;
	if (global) {
		if (::isTrash)
			cout << "trash:" << ::currentDir << ">";
		else
			cout << "disk:" << ::currentDir << ">";
	}
	getline(cin, input, '\n');
	// �������� �������� � ������ � �����
	size_t first = input.find_first_not_of(' ');
	if (string::npos == first)
		return input;
	size_t last = input.find_last_not_of(' ');
	input = input.substr(first, (last - first + 1));
	// �������� ������ �������� ����� �������
	input.erase(
		unique(input.begin(), input.end(),
			[](const char &a, const char &b) { return &a == &b || (a == ' ' && b == ' '); }),
		input.end());
	// ������ \ �� /
	replace(input.begin(), input.end(), '\\', '/');
	return input;
}

void authorize(YandexDisk &yandexDisk, string &token, string &refreshToken) {
	cout << "����������� ��� �� �������� �����" << endl;
	Token data;
	bool flag = false;

	if (!refreshToken.empty()) {
		data = yandexDisk.authorizationByRefresh(refreshToken);
		if (yandexDisk.getError() != "invalid_grant") {
			token = data.token;
			WritePrivateProfileString("common", "access_token", data.token.c_str(), configRef.c_str());
			WritePrivateProfileString("common", "refresh_token", data.refreshToken.c_str(), configRef.c_str());
			cout << "����� ����� ������� �������" << endl;
			return;
		}
	}

	if (!getUserAnsw("��� ��������� ������ ������ ����� ��� �������������. ������� �������?\n����� ���������� ���������"))
		exit(0);

	ShellExecute(NULL, "open", [](string a, string b) {
		return a + b;
	}("https://oauth.yandex.ru/authorize?response_type=code&client_id=", clientId).c_str(), NULL, NULL, SW_SHOW);

	while (!flag) {
		cout << "������� ��� �������������: ";
		input = getStr();
		data = yandexDisk.authorizationByCode(input);
		if (yandexDisk.getError() == "bad_verification_code" || yandexDisk.getError()== "invalid_grant")
			cout << "������: �������� ���! ���������� ��� ���" << endl;
		else
			flag = true;
	}

	token = data.token;
	WritePrivateProfileString("common", "access_token", data.token.c_str(), configRef.c_str());
	WritePrivateProfileString("common", "refresh_token", data.refreshToken.c_str(), configRef.c_str());
	cout << "����� ����� ������� �������!" << endl;
}

string convertBytes(double bytes) {
	int counter = 0;
	string size;
	while (bytes > 1024) {
		bytes = bytes / 1024;
		counter++;
	}
	bytes = static_cast<int>(bytes * 100 + 0.5) / 100.0;
	stringstream ss(stringstream::in | stringstream::out);
	ss << bytes;
	size = ss.str();
	switch (counter) {
	case 0:
		size += " �";
		break;
	case 1:
		size += " ��";
		break;
	case 2:
		size += " ��";
		break;
	case 3:
		size += " ��";
		break;
	case 4:
		size += " ��";
		break;
	}
	return size;
}