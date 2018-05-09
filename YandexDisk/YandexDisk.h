#pragma once
#include "stdafx.h"

namespace yandexdisk {
	struct File {
		std::string name;
		std::string type;
		int size;
		tm created;
		tm modified;

		File() : name(), type(), size() {}
	};

	struct Token {
		std::string token;
		std::string refreshToken;

		Token() : token(), refreshToken() {}
	};

	class YandexDisk {

	private:
		std::string clientId_, clientSecret_, tokenHeader_, error_, currentOperation_;
		int(*progressFunc)(void*, double, double, double, double);

		// Запись ответа на HTTP запрос в переменную
		static size_t writeData(char *ptr, size_t size, size_t nmemb, std::string* data);

		// Чтение содержимого файла для PUT запроса
		static size_t readСallback(void *ptr, size_t size, size_t nmemb, FILE *stream);

	public:
		YandexDisk(std::string CLIENT_ID_, std::string CLIENT_SECRET_);
		YandexDisk(std::string CLIENT_ID_, std::string CLIENT_SECRET_);
		~YandexDisk();

		/*-------------------------
		НАСТРОЙКА И ВЫВОД ПОЛЕЙ
		-------------------------*/

		// Ввести токен
		void setToken(std::string TOKEN_);

		// Удалить токен
		void deleteToken();

		// Вывести ошибку
		std::string getError();

		// Колбэк функция прогресса
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// Удалить функцию прогресса
		void deleteProgressFunc();


		/*-------------------------
		CURL ЗАПРОСЫ
		-------------------------*/

		// POST запрос cо скрытыми полями
		std::string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST запрос
		std::string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT запрос
		std::string putQuery(const char href[], const char fileLink[], const char headers[] = "");


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
		void uploadFileFromUrl(std::string from, std::string to);

		/*-------------------------
		ИЗМЕНЕНИЕ КОДИРОВКИ
		-------------------------*/

		// Перевод из UTF-8 в Windows-1251
		std::string utf8ToCp1251(const char *str);

		// Перевод из Windows-1251 в UTF-8
		std::string cp1251ToUtf8(const char *str);

	};

};