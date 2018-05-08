#pragma comment(lib, "curllib.lib")
#pragma comment(lib, "Shell32.lib")
#include "YandexDisk.h"
#include "Functions.h"
#include "Common.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <shlobj.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <Windows.h>
#include <iterator>
#include <time.h>
#include <iomanip>
using namespace std;
using namespace yandexdisk;

bool isTrash;
string currentDir = "/", dataRef, configRef, clientId, clientSecret, token, refreshToken;
vector<File> fileList;
const int helpWidth = 22;

int main(int argc, char* argv[]) {
	funcs["name"];

	return;

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	cout.setf(ios::left);

	char buf[MAX_PATH];
	GetCurrentDirectory(sizeof(buf), buf);
	dataRef = buf;
	dataRef += "\\YandexDiskClient";
	configRef = dataRef + "\\common.ini";

	ifstream file(configRef);
	if (!file.is_open()) {
		if (!CreateDirectory(dataRef.c_str(), NULL)) {
			cout << "Ошибка: Невозможно создать папку для конфигурации!" << endl;
			cout << "Путь : " << dataRef << endl;
			system("pause");
			return 0;
		}
		ofstream file(configRef);
		if (!file.is_open()) {
			cout << "Ошибка: Невозможно создать файл конфигурации!" << endl;
			cout << "Путь : " << configRef << endl;
			system("pause");
			return 0;
		}
		file << "[common]" << endl;
		file << "client_id=" << endl;
		file << "client_secret=" << endl;
	}
	file.close();

	char str[255];
	GetPrivateProfileString("common", "client_id", NULL, str, sizeof(str), configRef.c_str());
	clientId = str; // В функцию нельзя передать string
	GetPrivateProfileString("common", "client_secret", NULL, str, sizeof(str), configRef.c_str());
	clientSecret = str;
	GetPrivateProfileString("common", "access_token", NULL, str, sizeof(str), configRef.c_str());
	token = str;
	GetPrivateProfileString("common", "refresh_token", NULL, str, sizeof(str), configRef.c_str());
	refreshToken = str;

	if (clientId.empty() && clientSecret.empty())
	{
		cout << "Ошибка: не указан clientId или clientSecret!" << endl;
		if (!getUserAnsw("Вы хотите ввести их сейчас?"))
			return 0;
	}

	YandexDisk yandexDisk(clientId, clientSecret);

	if (token.empty())
		authorize(yandexDisk, token, refreshToken);

	yandexDisk.setToken(token);
	yandexDisk.setProgressFunc(progress);

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
			continue;
		size_t last = input.find_last_not_of(' ');
		input = input.substr(first, (last - first + 1));
		// Удаление лишних пробелов между словами
		input.erase(
			unique(input.begin(), input.end(),
				[](const char &a, const char &b) { return &a == &b || (a == ' ' && b == ' '); }),
			input.end());
		// Замена \ на /
		replace(input.begin(), input.end(), '\\', '/');



		// Вызод из приложения
		if (findInInput(input, "exit"))
			return 0;

		// Вывод справки
		if (findInInput(input, "help")) {
			cout << setw(helpWidth) << "exit" << "выход из приложения" << endl;
			cout << setw(helpWidth) << "help" << "список команд" << endl;
			cout << setw(helpWidth) << "space info" << "объём данных на диске" << endl;
			cout << setw(helpWidth) << "logout" << "выйти из учётной записи" << endl;
			cout << setw(helpWidth) << "cd %path%" << "сменить директорию" << endl;
			cout << setw(helpWidth) << "ls" << "список файлов в директории" << endl;
			cout << setw(helpWidth) << "open %path%" << "открыть фалй" << endl;
			cout << setw(helpWidth) << "clear cache" << "очистить кэш" << endl;
			cout << setw(helpWidth) << "download %from% %to%" << "загрузить ресурс" << endl;
			cout << setw(helpWidth) << "mkdir %path%" << "создать папку" << endl;
			cout << setw(helpWidth) << "delete %path%" << "удалить ресурс" << endl;
			cout << setw(helpWidth) << "config" << "открыть папку с настройками" << endl;
			cout << setw(helpWidth) << "upload %from% %to%" << "загрузить файл с компьютера на диск" << endl;
			cout << setw(helpWidth) << "upload %url% %to%" << "загрузить файл из интернета на диск" << endl;
			cout << setw(helpWidth) << "disk" << "переместиться в диск" << endl;
			cout << setw(helpWidth) << "trash" << "переместиться в корзину" << endl;
			cout << setw(helpWidth) << "recover %name%" << "восстановить файл из корзины под прежним названием" << endl;
			cout << setw(helpWidth) << "recover %name% &to%" << "восстановить файл из корзины под новым названием" << endl;
		}

		// Открыть папку с настройками и кэшэм
		else if (findInInput(input, "config")) {
			ShellExecute(0, "open", dataRef.c_str(), NULL, NULL, SW_SHOW);
		}

		// Данные о доступном объёме на диске
		else if (findInInput(input, "space info")) {
			// Если у 1 запроса не возникнет ошибки, то скорее всего у остальных тоже не возникнет
			float data = yandexDisk.getDiskData("total_space");
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			while (yandexDisk.getError() == "UnauthorizedError") {
				authorize(yandexDisk, token, refreshToken);
				data = yandexDisk.getDiskData("total_space");
			}

			cout << "Всего: " << convertBytes(data) << endl;
			cout << "Занято: " << convertBytes(yandexDisk.getDiskData("used_space")) << endl;
			cout << "В корзине: " << convertBytes(yandexDisk.getDiskData("trash_size")) << endl;
		}

		// Выйти из учётной записи яндекса
		else if (findInInput(input, "logout")) {
			if (!getUserAnsw("Вы уверены?"))
				continue;
			WritePrivateProfileString("common", "access_token", NULL, configRef.c_str());
			WritePrivateProfileString("common", "refresh_token", NULL, configRef.c_str());
			yandexDisk.deleteToken();
			token = "";
			refreshToken = "";
			cout << "Выход выполнен!" << endl;
		}

		// Список файлов в текущей директории
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Директория \"" << currentDir << "\" больше не существует" << endl;
				continue;
			}

			if (fileList.empty()) {
				cout << "В данной папке нет файлов" << endl;
				continue;
			}

			cout << setw(7) << "Тип";
			cout << setw(12) << "Размер";
			cout << setw(20) << "Создан";
			cout << setw(20) << "Изменён";
			cout << "Название" << endl;

			for (size_t i = 0; i < fileList.size(); i++) {
				char b[80];

				cout << setw(7) << fileList[i].type; // Тип

				cout << setw(12) << ((!fileList[i].size) ? "" : convertBytes(static_cast<double>(fileList[i].size))); // Размер

				strftime(b, 80, "%d %b %Y %H:%M", &fileList[i].created);
				cout << setw(20) << b; // Дата создания

				strftime(b, 80, "%d %b %Y %H:%M", &fileList[i].modified);
				cout << setw(20) << b; // Дата изменения

				cout << fileList[i].name; // Название

				cout << endl;
			}
		}

		// Сменить директорию
		else if (findInInput(input, "cd")) {
			if (isTrash) {
				cout << "Ошибка: В корзине нельзя перемещаться по папкам" << endl;
				continue;
			}

			input.erase(0, 3);

			if (!input.size()) {
				cout << "Ошибка: Вы не ввели адрес директории" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}

			currentDir = input;
		}

		// Переместиться в корзину
		else if (findInInput(input, "trash")) {
			if (isTrash) {
				cout << "Ошибка: Вы уже в корзине" << endl;
				continue;
			}

			isTrash = true;
			currentDir = "/";
		}

		// Переместиться в диск
		else if (findInInput(input, "disk")) {
			if (!isTrash) {
				cout << "Ошибка: Вы уже в диске" << endl;
				continue;
			}

			isTrash = false;
			currentDir = "/";
		}

		// Открыть файл
		else if (findInInput(input, "open")) {
			if (isTrash) {
				cout << "Ошибка: Нельзя открыть файл из корзины" << endl;
				continue;
			}

			input.erase(0, 5);

			if (!input.size()) {
				cout << "Ошибка: Вы не ввели адрес открываемого файла" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (type == "dir") {
				cout << "Ошибка: Просмотреть можно только файлы" << endl;
				cout << "Для перехода в директорию воспользуйтесь командой cd имя_директории" << endl;
				continue;
			}

			ifstream file(path);
			if (file.is_open()) {
				file.close();
				cout << "Файл откроется только если установленна программа по умолчанию для этого типа" << endl;
				ShellExecute(0, "open", path.c_str(), NULL, NULL, SW_SHOW);
				continue;
			}

			yandexDisk.downloadFile(input, path);

			cout << "Файл откроется только если установленна программа по умолчанию для этого типа" << endl;

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

		// Очистить кэш
		else if (findInInput(input, "clear cache")) {
			vector<vector<string>> list = getFileList([](string a, string b) { return a + b; }(dataRef, "\\*").c_str());

			for (size_t i = 0; i < list.size(); i++) {
				if (list[i][0] == "common.ini")
					continue;
				list[i][0] = dataRef + "\\" + list[i][0];
				remove(list[i][0].c_str());
			}

			cout << "Удалено файлов: " << list.size() - 1 << endl;
		}

		// Скачать файл
		else if (findInInput(input, "download")) {
			if (isTrash) {
				cout << "Ошибка: Нельзя скачать файл из корзины" << endl;
				continue;
			}

			input.erase(0, 9);

			if (split(input, " ").empty()) {
				cout << "Ошибка: Сликшом мало параметров" << endl;
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
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (yandexDisk.getError() == "Couldn't resolve host name") {
				cout << "Ошибка соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (type == "dir") {
				cout << "Ошибка: Загрузить можно только файлы" << endl;
				continue;
			}
			ifstream file(to);
			if (file.is_open() && !getUserAnsw("Такой файл уже существует. Перезаписать?")) {
				file.close();
				continue;
			}

			yandexDisk.downloadFile(from, to);

			if (yandexDisk.getError() == "Unable to open file") {
				cout << "Ошибка: Невозможно найти локальный файл по указанному адресу" << endl;
				continue;
			}

			cout << "Загружено" << endl;
		}

		// Создать директорию
		else if (findInInput(input, "mkdir")) {
			if (isTrash) {
				cout << "Ошибка: Нельзя создать директорию в корзине" << endl;
				continue;
			}

			input.erase(0, 6);

			if (!input.size()) {
				cout << "Ошибка: Вы не ввели адрес создаваемой директории" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskPathPointsToExistentDirectoryError") {
				cout << "Ошибка: Данная папка уже существует" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskPathDoesntExistsError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}

			cout << "Папка по адресу \"" << input << "\" создана" << endl;
		}

		// Удалить папку или файл
		else if (findInInput(input, "delete")) {
			input.erase(0, 7);

			if (!input.size()) {
				cout << "Ошибка: Вы не ввели адрес удаляемого файла" << endl;
				continue;
			}

			if (input.substr(0, 1) == "\"")
				input = split(input, "\"")[1];

			if (!getUserAnsw("Вы уверены?"))
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "Удаление началось. Процесс может занять продолжительное время" << endl;
			else
				cout << "Удалено!" << endl;
		}

		// Переместить папку или файл
		else if (findInInput(input, "move")) {
			if (isTrash) {
				cout << "Ошибка: Для восстановления файла из корзины воспользуйтесь командой recover" << endl;
				continue;
			}

			input.erase(0, 5);

			if (split(input, " ").empty()) {
				cout << "Ошибка: Сликшом мало параметров" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("Такой ресурс уже существует. Перезаписать?"))
					continue;

				yandexDisk.moveResource(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "Перемещение началось. Процесс может занять продолжительное время" << endl;
			else
				cout << "Ресурс успешно перемещён" << endl;
		}

		// Скопировать папку или файл
		else if (findInInput(input, "copy")) {
			if (isTrash) {
				cout << "Ошибка: Для восстановления файла из корзины воспользуйтесь командой recover" << endl;
				continue;
			}

			input.erase(0, 5);

			if (split(input, " ").empty()) {
				cout << "Ошибка: Сликшом мало параметров" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("Такой ресурс уже существует. Перезаписать?"))
					continue;

				yandexDisk.copyResourse(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "Копирование началось. Процесс может занять продолжительное время" << endl;
			else
				cout << "Ресурс успешно скопирован" << endl;
		}

		// Загрузить файл
		else if (findInInput(input, "upload")) {
			if (isTrash) {
				cout << "Нельзя загрузить файл в корзину" << endl;
				continue;
			}

			input.erase(0, 7);

			if (split(input, " ").empty()) {
				cout << "Ошибка: Сликшом мало параметров" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (yandexDisk.getError() == "Unable to open file") {
				cout << "Ошибка: Невозможно найти локальный файл по указанному адресу" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("Такой ресурс уже существует. Перезаписать?"))
					continue;

				yandexDisk.uploadFile(from, to, true);
			}
			if (!yandexDisk.getOperationStatus().empty())
				cout << "Загрука началась. Процесс может занять продолжительное время" << endl;
			else
				cout << "Загружено" << endl;
		}

		// Восстановление файла из корзины
		else if (findInInput(input, "recover")) {
			if (!isTrash) {
				cout << "Ошибка: Команда recover действует только в корзине" << endl;
				continue;
			}

			input.erase(0, 8);

			if (!input.size()) {
				cout << "Ошибка: Вы не ввели адрес удаляемого файла" << endl;
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
				cout << "Ошибка: Нет соединения с сервером. Попробуйте ещё раз" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskNotFoundError") {
				cout << "Ошибка: Указанный путь не найден" << endl;
				continue;
			}
			if (yandexDisk.getError() == "DiskResourceAlreadyExistsError") {

				if (!getUserAnsw("Такой ресурс уже существует. Перезаписать?"))
					continue;

				yandexDisk.recoverResoure(from, to, true);
			}

			if (!yandexDisk.getOperationStatus().empty())
				cout << "Восстановление началось. Процесс может занять продолжительное время" << endl;
			else
				cout << "Ресурс успешно восстановлен" << endl;
		}

		else
			cout << "Ошибка: Неизвестная команда" << endl;
	}
}

int progress(void *p, double dltotal, double dlnow, double ultotal, double ulnow) {

	if (dltotal > 0) {
		if (dltotal == dlnow)
			printf("\r%15c\r", ' ');
		else
			cout << "\rЗагрузка: " << int(dlnow * 100 / dltotal) << "%";
	}

	else if (ultotal > 0) {
		if (ultotal == ulnow)
			printf("\r%15c\r", ' ');
		else
			cout << "\rЗагрузка: " << int(ulnow * 100 / ultotal) << "%";
	}

	return 0;
}

bool findInInput(string input, string command) {
	transform(input.begin(), input.end(), input.begin(), tolower);
	int pos = input.find(command);
	if (pos != string::npos && pos == 0)
		return true;
	return false;
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

void authorize(YandexDisk &yandexDisk, string &token, string &refreshToken) {
	cout << "Отсутствует или не работает токен" << endl;
	Token data;
	bool flag = false;

	if (!refreshToken.empty()) {
		data = yandexDisk.authorizationByRefresh(refreshToken);
		if (yandexDisk.getError() != "invalid_grant") {
			token = data.token;
			WritePrivateProfileString("common", "access_token", data.token.c_str(), configRef.c_str());
			WritePrivateProfileString("common", "refresh_token", data.refreshToken.c_str(), configRef.c_str());
			cout << "Новый токен успешно получен" << endl;
			return;
		}
	}

	if (!getUserAnsw("Для получения нового токена нужен код подтверждения. Открыть браузер?\nИначе приложение закроется"))
		exit(0);

	ShellExecute(NULL, "open", [](string a, string b) {
		return a + b;
	}("https://oauth.yandex.ru/authorize?response_type=code&client_id=", clientId).c_str(), NULL, NULL, SW_SHOW);

	while (!flag) {
		cout << "Введите код подтверждения: ";
		input = getStr();
		data = yandexDisk.authorizationByCode(input);
		if (yandexDisk.getError() == "bad_verification_code" || yandexDisk.getError()== "invalid_grant")
			cout << "Ошибка: Неверный код! Попробуйте ещё раз" << endl;
		else
			flag = true;
	}

	token = data.token;
	WritePrivateProfileString("common", "access_token", data.token.c_str(), configRef.c_str());
	WritePrivateProfileString("common", "refresh_token", data.refreshToken.c_str(), configRef.c_str());
	cout << "Новый токен успешно получен!" << endl;
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
		size += " б";
		break;
	case 1:
		size += " Кб";
		break;
	case 2:
		size += " Мб";
		break;
	case 3:
		size += " Гб";
		break;
	case 4:
		size += " Тб";
		break;
	}
	return size;
}