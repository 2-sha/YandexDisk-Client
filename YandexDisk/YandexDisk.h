#pragma once
#pragma comment(lib,"curllib.lib")
#include <string>
#include <iostream>
#include <vector>
#include "curl\curl.h"
#include "curl\json.hpp" // Взято с https://habrahabr.ru/company/infopulse/blog/254075/

namespace yandedisk
{
	struct File 
	{
		std::string name;
		std::string type;
		int size;
		tm created;
		tm modified;

		File() : name(), type(), size(), created(), modified() {}
	};

	struct Token 
	{
		std::string token;
		std::string refreshToken;
	};

	class RequestException : public std::exception
	{
	public:
		RequestException(const char* msg);
	};

	class YandexDiskException : public std::exception
	{
	public:
		YandexDiskException(std::string description, std::string error);
		std::string getError();
	private:
		std::string error;
	};

	class YandexDisk
	{
	private:
		const char *clientId, *clientSecret;
		Token token;
		int(*progressFunc)(void*, double, double, double, double);

		/*-------------------------
		CURL ЗАПИСЬ И ЧТЕНИЕ
		-------------------------*/

		// Запись ответа на HTTP запрос в переменную
		static size_t writeData(char *ptr, size_t size, size_t nmemb, std::string* data);

		// Чтение содержимого файла для PUT запроса
		static size_t readData(void *ptr, size_t size, size_t nmemb, FILE *stream);


		/*-------------------------
		ИЗМЕНЕНИЕ КОДИРОВКИ
		-------------------------*/

		// Перевод из UTF-8 в Windows-1251
		std::string utf8ToCp1251(const char *str);

		// Перевод из Windows-1251 в UTF-8
		std::string cp1251ToUtf8(const char *str);


		/*-------------------------
		CURL ЗАПРОСЫ
		-------------------------*/

		// POST запрос cо скрытыми полями
		std::string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST запрос
		std::string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT запрос
		std::string putQuery(const char href[], const char fileLink[], const char headers[] = "");

	public:
		YandexDisk(std::string clientId, std::string clientSecret);
		YandexDisk(std::string clientId, std::string clientSecret, Token token);
		~YandexDisk();


		/*-------------------------
		НАСТРОЙКА И ВЫВОД ПОЛЕЙ
		-------------------------*/

		// Ввести токен
		void setToken(Token token);

		// Удалить токен
		void deleteToken();

		// Установить функцию прогресса
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// Удалить функцию прогресса
		void deleteProgressFunc();


		/*-------------------------
		АВТОРИЗАЦИЯ
		-------------------------*/

		// Авторизация по коду
		Token authorizationByCode(std::string code);

		// Авторизация по refresh токену
		Token authorizationByRefresh(std::string refreshToken);


		/*-------------------------
		ОПЕРАЦИИ НАД РЕСУРСАМИ
		-------------------------*/

		// Создание директории
		void makeDir(std::string path);

		// Удаление файла\папки
		void deleteResource(std::string path);

		// Очистить корзину
		void deleteTrash(std::string path);

		// Переместить ресурс
		void moveResource(std::string from, std::string to, bool overwrite);

		// Копировать ресурс
		void copyResourse(std::string from, std::string to, bool overwrite);

		// Восстановить ресурс из корины
		void recoverResoure(std::string from, std::string to, bool overwrite);


		/*-------------------------
		ПОЛУЧЕНИЕ ИНФОРМАЦИИ
		-------------------------*/

		// Получить информацию о диске
		float getDiskData(std::string type);

		// Список файлов в папке
		std::vector<File> getFileList(std::string path);

		// Список файлов в корзине
		std::vector<File> getTrashList(std::string path);

		// Статус операции
		std::string getOperationStatus();

		// Получить метаинформацию о ресурсе
		File getMetaInfo(std::string path);


		/*-------------------------
		ЗАГРУЗКА И СКАЧИВАНИЕ ФАЙЛОВ
		-------------------------*/

		// Скачивание файла
		void downloadFile(std::string from, std::string to);

		// Загрузка файла
		void uploadFile(std::string from, std::string to, bool overwrite);

		// Загрузка файла из интернета
		void uploadFileFromLink(std::string from, std::string to);
	};
}