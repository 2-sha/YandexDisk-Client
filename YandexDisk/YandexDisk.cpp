#include "YandexDisk.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "curl/curl.h"
#include "curl/json.hpp" // https://habrahabr.ru/company/infopulse/blog/254075/
using namespace std;
using namespace yandexdisk;
using json = nlohmann::json;


size_t YandexDisk::writeData(char *ptr, size_t size, size_t nmemb, string* data) {
	if (data) {
		data->append(ptr, size*nmemb);
		return size*nmemb;
	}
	else
		return 0;
};

size_t YandexDisk::read—allback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	curl_off_t nread;
	size_t retcode = fread(ptr, size, nmemb, stream);
	nread = (curl_off_t)retcode;
	return retcode;
};


YandexDisk::YandexDisk(string clientId, string clientSecret)
	: clientId_(clientId), clientSecret_(clientSecret) {}

YandexDisk::~YandexDisk() {}



/*-------------------------
Õ¿—“–Œ… ¿ » ¬€¬Œƒ œŒÀ≈…
-------------------------*/

void YandexDisk::setToken(string token) {
	tokenHeader_ = "Authorization: OAuth " + token;
}

void YandexDisk::deleteToken() {
	tokenHeader_ = "";
}

string YandexDisk::getError() {
	return error_;
}

void YandexDisk::setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow)) {
	progressFunc = f;
}

void YandexDisk::deleteProgressFunc() {
	progressFunc = NULL;
}


/*-------------------------
CURL «¿œ–Œ—€
-------------------------*/

string YandexDisk::postQuery(const char href[], const char postFields[], const char headers[]) {
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	string body;

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, href);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		if (progressFunc) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			json j;
			j["error"] = curl_easy_strerror(res);
			body = j.dump();
		}

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}

string YandexDisk::restQuery(const char href[], const char type[], const char headers[]) {
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	string body;

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, href);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, type);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		if (progressFunc) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			json j;
			j["error"] = curl_easy_strerror(res);
			body = j.dump();
		}

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}

string YandexDisk::putQuery(const char href[], const char fileLink[], const char headers[]) {
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	struct stat file_info;
	string body;

	FILE *fd;
	fopen_s(&fd, fileLink, "rb");
	fstat(_fileno(fd), &file_info);

	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, href);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_READDATA, fd);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read—allback);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
		if (progressFunc) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			json j;
			j["error"] = curl_easy_strerror(res);
			body = j.dump();
		}

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}


/*-------------------------
¿¬“Œ–»«¿÷»ﬂ
-------------------------*/

Token YandexDisk::authorizationByCode(string code) {
	error_ = "";
	string post = "grant_type=authorization_code&code=" + code + "&client_id=" + clientId_ + "&client_secret=" + clientSecret_;
	json j = json::parse(postQuery("https://oauth.yandex.ru/token", post.c_str()));
	Token data;
	if (j.find("access_token") != j.end()) {
		data.token = j["access_token"];
		data.refreshToken = j["refresh_token"];
		tokenHeader_ = "Authorization: OAuth " + data.token;
	}
	else {
		error_ = j["error"];
	}
	return data;
}

Token YandexDisk::authorizationByRefresh(string refreshToken) {
	error_ = "";
	string post = "grant_type=refresh_token&refresh_token=" + refreshToken + "&client_id=" + clientId_ + "&client_secret=" + clientSecret_;
	json j = json::parse(postQuery("https://oauth.yandex.ru/token", post.c_str()));
	Token data;
	if (j.find("access_token") != j.end()) {
		data.token = j["access_token"];
		data.refreshToken = j["refresh_token"];
		tokenHeader_ = "Authorization: OAuth " + data.token;
	}
	else {
		error_ = j["error"];
	}
	return data;
}


/*-------------------------
Œœ≈–¿÷»» Õ¿ƒ –≈—”–—¿Ã»
-------------------------*/

void YandexDisk::makeDir(string path) {
	error_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net:443/v1/disk/resources?path=" + path;
	json j = json::parse(restQuery(path.c_str(), "PUT", tokenHeader_.c_str()));
	if (j.find("error") != j.end())
		error_ = j["error"];
}

void YandexDisk::deleteResource(string path) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net:443/v1/disk/resources?path=" + path;
	string answ = restQuery(path.c_str(), "DELETE", tokenHeader_.c_str());
	if (answ.empty())
		return;
	json j = json::parse(answ);
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
	}
	else
		error_ = j["error"];
}

void YandexDisk::deleteTrash(string path) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net/v1/disk/trash/resources?path=" + path;
	string answ = restQuery(path.c_str(), "DELETE", tokenHeader_.c_str());
	if (answ.empty())
		return;
	json j = json::parse(answ);
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
	}
	else
		error_ = j["error"];
}

void YandexDisk::moveResource(string from, string to, bool overwrite) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	from = cp1251ToUtf8(from.c_str());
	from = curl_easy_escape(curl, from.c_str(), from.length());
	to = cp1251ToUtf8(to.c_str());
	to = curl_easy_escape(curl, to.c_str(), to.length());
	string path = "https://cloud-api.yandex.net/v1/disk/resources/move?from="
		+ from + "&path="
		+ to + "&overwrite=" +
		+ ((overwrite) ? "true" : "false");
	json j = json::parse(restQuery(path.c_str(), "POST", tokenHeader_.c_str()));
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		if (currentOperation_.find("path") == string::npos)
			currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
		else
			currentOperation_ = "";
	}
	else
		error_ = j["error"];
}

void YandexDisk::copyResourse(string from, string to, bool overwrite) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	from = cp1251ToUtf8(from.c_str());
	from = curl_easy_escape(curl, from.c_str(), from.length());
	to = cp1251ToUtf8(to.c_str());
	to = curl_easy_escape(curl, to.c_str(), to.length());
	string path = "https://cloud-api.yandex.net/v1/disk/resources/copy?from="
		+ from + "&path="
		+ to + "&overwrite=" +
		+((overwrite) ? "true" : "false");
	json j = json::parse(restQuery(path.c_str(), "POST", tokenHeader_.c_str()));
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		if (currentOperation_.find("path") == string::npos)
			currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
		else
			currentOperation_ = "";
	}
	else
		error_ = j["error"];
}

void YandexDisk::recoverResoure(string from, string to, bool overwrite) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	from = cp1251ToUtf8(from.c_str());
	from = curl_easy_escape(curl, from.c_str(), from.length());
	string path = "https://cloud-api.yandex.net/v1/disk/trash/resources/restore?path=" + from;
	if (!to.empty()) {
		to = cp1251ToUtf8(to.c_str());
		to = curl_easy_escape(curl, to.c_str(), to.length());
		path += "&name="+ to + "&overwrite=" +((overwrite) ? "true" : "false");
	}
	json j = json::parse(restQuery(path.c_str(), "PUT", tokenHeader_.c_str()));
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		if (currentOperation_.find("path") == string::npos)
			currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
		else
			currentOperation_ = "";
	}
	else
		error_ = j["error"];
}


/*-------------------------
œŒÀ”◊≈Õ»≈ »Õ‘Œ–Ã¿÷»»
-------------------------*/

float YandexDisk::getDiskData(string type) {
	error_ = "";
	json j = json::parse(restQuery("https://cloud-api.yandex.net/v1/disk/", "GET", tokenHeader_.c_str()));
	if (j.find(type) != j.end())
		return j[type];
	else
		error_ = j["error"];
	return 0;
}

vector<File> YandexDisk::getFileList(string path) {
	error_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net/v1/disk/resources?path=" + path + "&limit=1000";
	json j = json::parse(restQuery(path.c_str(), "GET", tokenHeader_.c_str()));
	vector<File> data;
	if (j.find("_embedded") != j.end()) {
		int size = j["_embedded"]["items"].size();
		if (size) {
			data.resize(size);
			for (int i = 0; i < size; i++) {
				string s;
				struct tm timeinfo;

				data[i].name = j["_embedded"]["items"][i]["name"];

				data[i].name = utf8ToCp1251(data[i].name.c_str());

				data[i].type = j["_embedded"]["items"][i]["type"];

				data[i].size = ((j["_embedded"]["items"][i]["size"].dump() == "null") ? 0 :
					atoi(j["_embedded"]["items"][i]["size"].dump().c_str()));

				s = j["_embedded"]["items"][i]["created"];
				timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
				timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
				timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
				timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
				mktime(&timeinfo);
				data[i].created = timeinfo;

				s = j["_embedded"]["items"][i]["modified"];
				timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
				timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
				timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
				timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
				mktime(&timeinfo);

				data[i].modified = timeinfo;
			}
		}
	}
	else {
		error_ = j["error"];
	}
	return data;
}

vector<File> YandexDisk::getTrashList(string path) {
	error_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net/v1/disk/trash/resources?path=" + path + "&limit=1000";
	json j = json::parse(restQuery(path.c_str(), "GET", tokenHeader_.c_str()));
	vector<File> data;
	if (j.find("_embedded") != j.end()) {
		int size = j["_embedded"]["items"].size();
		if (size) {
			data.resize(size);
			for (int i = 0; i < size; i++) {
				string s;
				struct tm timeinfo;

				data[i].name = j["_embedded"]["items"][i]["path"];
				data[i].name.erase(0, 7);

				data[i].name = utf8ToCp1251(data[i].name.c_str());

				data[i].type = j["_embedded"]["items"][i]["type"];

				data[i].size = ((j["_embedded"]["items"][i]["size"].dump() == "null") ? 0 :
					atoi(j["_embedded"]["items"][i]["size"].dump().c_str()));

				s = j["_embedded"]["items"][i]["created"];
				timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
				timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
				timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
				timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
				mktime(&timeinfo);
				data[i].created = timeinfo;

				s = j["_embedded"]["items"][i]["modified"];
				timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
				timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
				timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
				timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
				timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
				timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
				mktime(&timeinfo);

				data[i].modified = timeinfo;
			}
		}
	}
	else {
		error_ = j["error"];
	}
	return data;
}

string YandexDisk::getOperationStatus() {
	error_ = "";
	if (currentOperation_.empty())
		return "";
	json j = json::parse(restQuery([](string a, string b) {
		return a + b;
	}("https://cloud-api.yandex.net/v1/disk/operations/", currentOperation_).c_str(), "GET", tokenHeader_.c_str()));
	return j["status"];
}

File YandexDisk::getMetaInfo(string path) {
	error_ = "";
	CURL *curl = curl_easy_init();
	path = cp1251ToUtf8(path.c_str());
	path = curl_easy_escape(curl, path.c_str(), path.length());
	path = "https://cloud-api.yandex.net/v1/disk/resources/?path=" + path;
	json j = json::parse(restQuery(path.c_str(), "PATCH", tokenHeader_.c_str()));
	File file;
	if (j.find("name") != j.end()) {
		string s;
		struct tm timeinfo;

		file.name = j["name"];

		file.name = utf8ToCp1251(file.name.c_str());

		file.type = j["type"];

		file.size = ((j["size"].dump() == "null") ? 0 :
			atoi(j["size"].dump().c_str()));

		s = j["created"];
		timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
		timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
		timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
		timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
		timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
		timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
		mktime(&timeinfo);
		file.created = timeinfo;

		s = j["modified"];
		timeinfo.tm_year = atoi(s.substr(0, 4).c_str()) - 1900;
		timeinfo.tm_mon = atoi(s.substr(5, 2).c_str()) - 1;
		timeinfo.tm_mday = atoi(s.substr(8, 2).c_str());
		timeinfo.tm_hour = atoi(s.substr(11, 2).c_str());
		timeinfo.tm_min = atoi(s.substr(14, 2).c_str());
		timeinfo.tm_sec = atoi(s.substr(17, 2).c_str());
		mktime(&timeinfo);

		file.modified = timeinfo;
	}
	else {
		error_ = j["error"];
	}
	return file;
}


/*-------------------------
«¿√–”« ¿ » — ¿◊»¬¿Õ»≈ ‘¿…ÀŒ¬
-------------------------*/

void YandexDisk::downloadFile(string from, string to) {
	error_ = "";
	string answ;
	bool b = false;
	CURL *curl = curl_easy_init();
	from = cp1251ToUtf8(from.c_str());
	from = curl_easy_escape(curl, from.c_str(), from.length());
	from = "https://cloud-api.yandex.net/v1/disk/resources/download?path=" + from;
	json j = json::parse(restQuery(from.c_str(), "GET", tokenHeader_.c_str()));
	if (j.find("href") != j.end()) {
		answ = j["href"];
		ofstream file(to, ios_base::binary);
		if (file.is_open()) {
			file << restQuery(answ.c_str(), "GET", tokenHeader_.c_str());
			file.close();
		}
		else
			error_ = "Unable to open file";
	}
	else {
		error_ = j["error"];
	}
}

void YandexDisk::uploadFile(string from, string to, bool overwrite) {
	error_ = "";
	currentOperation_ = "";
	ifstream file(from);
	if (!file.is_open()) {
		error_ = "Unable to open file";
		return;
	}
	file.close();
	CURL *curl = curl_easy_init();
	to = cp1251ToUtf8(to.c_str());
	to = curl_easy_escape(curl, to.c_str(), to.length());
	to = "https://cloud-api.yandex.net/v1/disk/resources/upload?path=" + to +
		"&overwrite=" + ((overwrite) ? "true" : "false");
	json j = json::parse(restQuery(to.c_str(), "GET", tokenHeader_.c_str()));
	if (j.find("error") != j.end()) {
		error_ = j["error"];
		return;
	}
	string s = j["href"];
	putQuery(s.c_str(), from.c_str(), tokenHeader_.c_str());
}

void YandexDisk::uploadFileFromUrl(string from, string to) {
	error_ = "";
	currentOperation_ = "";
	CURL *curl = curl_easy_init();
	to = cp1251ToUtf8(to.c_str());
	to = curl_easy_escape(curl, to.c_str(), to.length());
	to = "https://cloud-api.yandex.net/v1/disk/resources/upload?url=" + from
		+ "&path=" + to 
		+ "&disable_redirects=true";
	json j = json::parse(restQuery(to.c_str(), "POST", tokenHeader_.c_str()));
	if (j.find("href") != j.end()) {
		currentOperation_ = j["href"];
		currentOperation_ = currentOperation_.substr(currentOperation_.find_last_of("/") + 1);
	}
	else 
		error_ = j["error"];
}

/*-------------------------
»«Ã≈Õ≈Õ»≈  Œƒ»–Œ¬ »
-------------------------*/

string YandexDisk::utf8ToCp1251(const char *str) {
	string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	if (!result_u)
		return 0;
	wchar_t *ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(1251, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return 0;
	}
	char *cres = new char[result_c];
	if (!WideCharToMultiByte(1251, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

string YandexDisk::cp1251ToUtf8(const char *str) {
	string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
	if (!result_u)
		return 0;

	wchar_t *ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(CP_UTF8, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return 0;
	}
	char *cres = new char[result_c];
	if (!WideCharToMultiByte(CP_UTF8, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}