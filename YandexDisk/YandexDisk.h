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

		// ������ ������ �� HTTP ������ � ����������
		static size_t writeData(char *ptr, size_t size, size_t nmemb, std::string* data);

		// ������ ����������� ����� ��� PUT �������
		static size_t read�allback(void *ptr, size_t size, size_t nmemb, FILE *stream);

	public:
		YandexDisk(std::string CLIENT_ID_, std::string CLIENT_SECRET_);
		YandexDisk(std::string CLIENT_ID_, std::string CLIENT_SECRET_);
		~YandexDisk();

		/*-------------------------
		��������� � ����� �����
		-------------------------*/

		// ������ �����
		void setToken(std::string TOKEN_);

		// ������� �����
		void deleteToken();

		// ������� ������
		std::string getError();

		// ������ ������� ���������
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// ������� ������� ���������
		void deleteProgressFunc();


		/*-------------------------
		CURL �������
		-------------------------*/

		// POST ������ c� �������� ������
		std::string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST ������
		std::string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT ������
		std::string putQuery(const char href[], const char fileLink[], const char headers[] = "");


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
		void uploadFileFromUrl(std::string from, std::string to);

		/*-------------------------
		��������� ���������
		-------------------------*/

		// ������� �� UTF-8 � Windows-1251
		std::string utf8ToCp1251(const char *str);

		// ������� �� Windows-1251 � UTF-8
		std::string cp1251ToUtf8(const char *str);

	};

};