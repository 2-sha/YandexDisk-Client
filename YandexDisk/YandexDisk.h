#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;

namespace yandexdisk {
	struct File {
		string name;
		string type;
		int size;
		tm created;
		tm modified;

		File() : name(), type(), size() {}
	};

	struct Token {
		string token;
		string refreshToken;

		Token() : token(), refreshToken() {}
	};

	class YandexDisk {

	private:
		string clientId_, clientSecret_, tokenHeader_, error_, currentOperation_;
		int(*progressFunc)(void*, double, double, double, double);

		// Запись ответа на HTTP запрос в переменную
		static size_t writeData(char *ptr, size_t size, size_t nmemb, string* data);

		// Чтение содержимого файла для PUT запроса
		static size_t readСallback(void *ptr, size_t size, size_t nmemb, FILE *stream);

	public:
		YandexDisk(string CLIENT_ID_, string CLIENT_SECRET_);
		~YandexDisk();


		/*-------------------------
		НАСТРОЙКА И ВЫВОД ПОЛЕЙ
		-------------------------*/

		// Ввести токен
		void setToken(string TOKEN_);

		// Удалить токен
		void deleteToken();

		// Вывести ошибку
		string getError();

		// Колбэк функция прогресса
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// Удалить функцию прогресса
		void deleteProgressFunc();


		/*-------------------------
		CURL ЗАПРОСЫ
		-------------------------*/

		// POST запрос cо скрытыми полями
		string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST запрос
		string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT запрос
		string putQuery(const char href[], const char fileLink[], const char headers[] = "");


		/*-------------------------
		АВТОРИЗАЦИЯ
		-------------------------*/

		// Авторизация по коду
		Token authorizationByCode(string code);

		// Авторизация по refresh токену
		Token authorizationByRefresh(string refreshToken);


		/*-------------------------
		ОПЕРАЦИИ НАД РЕСУРСАМИ
		-------------------------*/

		// Создание директории
		void makeDir(string path);

		// Удаление файла\папки
		void deleteResource(string path);

		// Очистить корзину
		void deleteTrash(string path);

		// Переместить ресурс
		void moveResource(string from, string to, bool overwrite);

		// Копировать ресурс
		void copyResourse(string from, string to, bool overwrite);

		// Восстановить ресурс из корины
		void recoverResoure(string from, string to, bool overwrite);


		/*-------------------------
		ПОЛУЧЕНИЕ ИНФОРМАЦИИ
		-------------------------*/

		// Получить информацию о диске
		float getDiskData(string type);

		// Список файлов в папке
		vector<File> getFileList(string path);

		// Список файлов в корзине
		vector<File> getTrashList(string path);

		// Статус операции
		string getOperationStatus();

		// Получить метаинформацию о ресурсе
		File getMetaInfo(string path);


		/*-------------------------
		ЗАГРУЗКА И СКАЧИВАНИЕ ФАЙЛОВ
		-------------------------*/

		// Скачивание файла
		void downloadFile(string from, string to);

		// Загрузка файла
		void uploadFile(string from, string to, bool overwrite);

		// Загрузка файла из интернета
		void uploadFileFromUrl(string from, string to);

		/*-------------------------
		ИЗМЕНЕНИЕ КОДИРОВКИ
		-------------------------*/

		// Перевод из UTF-8 в Windows-1251
		string utf8ToCp1251(const char *str);

		// Перевод из Windows-1251 в UTF-8
		string cp1251ToUtf8(const char *str);

	};

};