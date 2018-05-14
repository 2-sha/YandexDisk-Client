#include "YandexDisk.h"
using namespace yandedisk;
using json = nlohmann::json;

YandexDiskException::YandexDiskException(std::string description, std::string error) :
	exception(description.c_str()), error(error)
{

}

std::string YandexDiskException::getError()
{
	return error;
}

RequestException::RequestException(const char *msg) :
	exception(msg)
{
}


YandexDisk::YandexDisk(std::string clientId, std::string clientSecret) :
	clientId(clientId.c_str()), clientSecret(clientSecret.c_str())
{
}

YandexDisk::YandexDisk(std::string clientId, std::string clientSecret, Token token) :
	clientId(clientId.c_str()), clientSecret(clientSecret.c_str()), token(token)
{
}

YandexDisk::~YandexDisk()
{
}


size_t YandexDisk::writeData(char *ptr, size_t size, size_t nmemb, std::string* data) 
{
	if (data) {
		data->append(ptr, size*nmemb);
		return size * nmemb;
	}
	else
		return 0;
};

size_t YandexDisk::readData(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	curl_off_t nread;
	size_t retcode = fread(ptr, size, nmemb, stream);
	nread = (curl_off_t)retcode;
	return retcode;
};


std::string YandexDisk::utf8ToCp1251(const char *str) {
	std::string res;
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

std::string YandexDisk::cp1251ToUtf8(const char *str) {
	std::string res;
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


std::string YandexDisk::postQuery(const char href[], const char postFields[], const char headers[]) {

	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	std::string body;

	if (curl)
	{
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
		if (res != CURLE_OK) 
			throw RequestException(curl_easy_strerror(res));

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}

std::string YandexDisk::restQuery(const char href[], const char type[], const char headers[]) 
{
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	std::string body;

	if (curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, href);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, type);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		if (progressFunc) 
		{
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			throw RequestException(curl_easy_strerror(res));

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}

std::string YandexDisk::putQuery(const char href[], const char fileLink[], const char headers[]) 
{
	CURL *curl = curl_easy_init();
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, headers);
	struct stat file_info;
	std::string body;

	FILE *fd;
	fopen_s(&fd, fileLink, "rb");
	fstat(_fileno(fd), &file_info);

	if (curl) 
	{

		curl_easy_setopt(curl, CURLOPT_URL, href);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_READDATA, fd);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, readData);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
		if (progressFunc) {
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressFunc);
		}

		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			throw RequestException(curl_easy_strerror(res));

		curl_slist_free_all(list);
		curl_easy_cleanup(curl);
	}
	return body;
}


void YandexDisk::setToken(Token token) 
{
	this->token = token;
}

void YandexDisk::deleteToken() 
{
	this->token.token = "";
	this->token.refreshToken = "";
}

void YandexDisk::setProgressFunc(int(*f)(void *p, double dltotal, double dlnow, double ultotal, double ulnow)) 
{
	progressFunc = f;
}

void YandexDisk::deleteProgressFunc() 
{
	progressFunc = nullptr;
}

Token YandexDisk::authorizationByCode(std::string code) 
{
	std::string post = "grant_type=authorization_code&code=" + code + "&client_id=" + clientId + "&client_secret=" + clientSecret;
	json j = json::parse(postQuery("https://oauth.yandex.ru/token", post.c_str()));
	Token data;
	if (j.find("access_token") != j.end()) 
	{
		data.token = token.token = j["access_token"];
		data.refreshToken = token.refreshToken = j["refresh_token"];
	}
	else 
	{
		throw YandexDiskException(j["error"], j["error_description"]);
	}
	return data;
}

Token YandexDisk::authorizationByRefresh(std::string refreshToken) 
{
	std::string post = "grant_type=refresh_token&refresh_token=" + refreshToken + "&client_id=" + clientId + "&client_secret=" + clientSecret;
	json j = json::parse(postQuery("https://oauth.yandex.ru/token", post.c_str()));
	Token data;
	if (j.find("access_token") != j.end()) 
	{
		data.token = token.token = j["access_token"];
		data.refreshToken = token.refreshToken = j["refresh_token"];
	}
	else 
	{
		throw YandexDiskException(j["error"], j["error_description"]);
	}
	return data;
}