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

		// ������ ������ �� HTTP ������ � ����������
		static size_t writeData(char *ptr, size_t size, size_t nmemb, string* data);

		// ������ ����������� ����� ��� PUT �������
		static size_t read�allback(void *ptr, size_t size, size_t nmemb, FILE *stream);

	public:
		YandexDisk(string CLIENT_ID_, string CLIENT_SECRET_);
		~YandexDisk();


		/*-------------------------
		��������� � ����� �����
		-------------------------*/

		// ������ �����
		void setToken(string TOKEN_);

		// ������� �����
		void deleteToken();

		// ������� ������
		string getError();

		// ������ ������� ���������
		void setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow));

		// ������� ������� ���������
		void deleteProgressFunc();


		/*-------------------------
		CURL �������
		-------------------------*/

		// POST ������ c� �������� ������
		string postQuery(const char href[], const char postFields[], const char headers[] = "");

		// REST ������
		string restQuery(const char href[], const char type[], const char headers[] = "");

		// PUT ������
		string putQuery(const char href[], const char fileLink[], const char headers[] = "");


		/*-------------------------
		�����������
		-------------------------*/

		// ����������� �� ����
		Token authorizationByCode(string code);

		// ����������� �� refresh ������
		Token authorizationByRefresh(string refreshToken);


		/*-------------------------
		�������� ��� ���������
		-------------------------*/

		// �������� ����������
		void makeDir(string path);

		// �������� �����\�����
		void deleteResource(string path);

		// �������� �������
		void deleteTrash(string path);

		// ����������� ������
		void moveResource(string from, string to, bool overwrite);

		// ���������� ������
		void copyResourse(string from, string to, bool overwrite);

		// ������������ ������ �� ������
		void recoverResoure(string from, string to, bool overwrite);


		/*-------------------------
		��������� ����������
		-------------------------*/

		// �������� ���������� � �����
		float getDiskData(string type);

		// ������ ������ � �����
		vector<File> getFileList(string path);

		// ������ ������ � �������
		vector<File> getTrashList(string path);

		// ������ ��������
		string getOperationStatus();

		// �������� �������������� � �������
		File getMetaInfo(string path);


		/*-------------------------
		�������� � ���������� ������
		-------------------------*/

		// ���������� �����
		void downloadFile(string from, string to);

		// �������� �����
		void uploadFile(string from, string to, bool overwrite);

		// �������� ����� �� ���������
		void uploadFileFromUrl(string from, string to);

		/*-------------------------
		��������� ���������
		-------------------------*/

		// ������� �� UTF-8 � Windows-1251
		string utf8ToCp1251(const char *str);

		// ������� �� Windows-1251 � UTF-8
		string cp1251ToUtf8(const char *str);

	};

};