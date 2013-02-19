#ifndef _CURL_H_
#define _CURL_H_
#include <curl/curl.h>
#include <stdarg.h>
#include <iostream>
#include <string>

using namespace std;
class Curl{
	public:
		Curl();
		~Curl();
	//public functions
	public:
		CURLcode post(const char * url,const char * content, const char *cookie = NULL);
		CURLcode get(const char * url, const char *cookie = NULL);
		const char * header(const char *);
		const char * header();
		struct curl_slist * header(const char *, struct curl_slist *);
		long status();
		const char * response(void);
		void proxy(const char *proxy, curl_proxytype type = CURLPROXY_HTTP,  const char *user = NULL, const char * pwd = NULL);


	//private functions
	private:
		CURLcode curl_perform();
		void curl_setopt(CURLoption opt, ...);
		static size_t header_callback( void *ptr, size_t size, size_t nmemb, std::string *userdata);
		static size_t write_callback( void *ptr, size_t size, size_t nmemb, std::string *response);
	//private varibales
	private:
		CURL *curl;
		long statusCode;
		std::string headerInfo;
		std::string responseInfo;

	protected:
};
#endif
