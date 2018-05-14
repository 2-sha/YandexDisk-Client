#pragma once
#pragma comment(lib,"curl/curllib.lib")
#include <string>
#include <iostream>
#include <vector>
#include "curl\curl.h"
#include "curl\json.hpp" // ����� � https://habrahabr.ru/company/infopulse/blog/254075/

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
		CURL ������ � ������
		-------------------------*/

		// ������ ������ �� HTTP ������ � ����������
		static size_t writeData(char *ptr, size_t size, size_t nmemb, std::string* data);

		// ������ ����������� ����� ��� PUT �������
		static size_t readData(void *ptr, size_t size, size_t nmemb, FILE *stream);


		/*-------------------------
		��������� ���������
		-------------------------*/

		// ������� �� UTF-8 � Windows-1251
		std::string utf8ToCp1251(const char *str);

		// ������� �� Windows-1251 � UTF-8
		std::string cp1251ToUtf8(const char *str);


		/*-------------------------
		CURL �������
		-------------------------*/

		// POST ������ c� �������� ������
		std::string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST ������
		std::string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT ������
		std::string putQuery(const char href[], const char fileLink[], const char headers[] = "");

	public:
		YandexDisk(std::string clientId, std::string clientSecret);
		YandexDisk(std::string clientId, std::string clientSecret, Token token);
		~YandexDisk();


		/*-------------------------
		��������� � ����� �����
		-------------------------*/

		// ������ �����
		void setToken(Token token);

		// ������� �����
		void deleteToken();

		// ���������� ������� ���������
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// ������� ������� ���������
		void deleteProgressFunc();


		/*-------------------------
		�����������
		-------------------------*/

		// ����������� �� ����
		Token authorizationByCode(std::string code);

		// ����������� �� refresh ������
		Token authorizationByRefresh(std::string refreshToken);


		/*-------------------------
		�������� ��� ���������
		-------------------------*/

		// �������� ����������
		void makeDir(std::string path);

		// �������� �����\�����
		void deleteResource(std::string path);

		// �������� �������
		void deleteTrash(std::string path);

		// ����������� ������
		void moveResource(std::string from, std::string to, bool overwrite);

		// ���������� ������
		void copyResourse(std::string from, std::string to, bool overwrite);

		// ������������ ������ �� ������
		void recoverResoure(std::string from, std::string to, bool overwrite);


		/*-------------------------
		��������� ����������
		-------------------------*/

		// �������� ���������� � �����
		float getDiskData(std::string type);

		// ������ ������ � �����
		std::vector<File> getFileList(std::string path);

		// ������ ������ � �������
		std::vector<File> getTrashList(std::string path);

		// ������ ��������
		std::string getOperationStatus();

		// �������� �������������� � �������
		File getMetaInfo(std::string path);


		/*-------------------------
		�������� � ���������� ������
		-------------------------*/

		// ���������� �����
		void downloadFile(std::string from, std::string to);

		// �������� �����
		void uploadFile(std::string from, std::string to, bool overwrite);

		// �������� ����� �� ���������
		void uploadFileFromLink(std::string from, std::string to);
	};
}