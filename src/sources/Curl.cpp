#include "Curl.h"
#include <string.h>
Curl::Curl(){
	this->curl = curl_easy_init();
	this->curl_setopt(CURLOPT_WRITEHEADER, &this->headerInfo);
	this->curl_setopt(CURLOPT_HEADERFUNCTION, &this->header_callback);
	this->curl_setopt(CURLOPT_WRITEDATA, &this->responseInfo);
	this->curl_setopt(CURLOPT_WRITEFUNCTION, &this->write_callback);
	this->curl_setopt(CURLOPT_FOLLOWLOCATION, 1L);
	this->curl_setopt(CURLOPT_TIMEOUT, 3L);
	this->curl_setopt(CURLOPT_COOKIEJAR, "./cookies");
	this->curl_setopt(CURLOPT_HEADER, 1L);
	this->statusCode = -1;
}
Curl::~Curl(){
	curl_easy_cleanup(this->curl);
}
void Curl::curl_setopt(CURLoption opt, ...){
	va_list  args;
	va_start(args, opt);
	curl_easy_setopt(this->curl,opt,va_arg(args,void *));
	va_end(args);
}
void Curl::proxy(const char *proxy, curl_proxytype type,  const char *user, const char * pwd){
	this->curl_setopt(CURLOPT_PROXY, proxy);
	switch(type){
		case CURLPROXY_HTTP:
			this->curl_setopt(CURLOPT_HTTPPROXYTUNNEL,true);
			this->curl_setopt(CURLOPT_PROXYTYPE,CURLPROXY_HTTP);
		break;
		case CURLPROXY_SOCKS4:
			this->curl_setopt(CURLOPT_PROXYTYPE,CURLPROXY_SOCKS4);
		break;
		case CURLPROXY_SOCKS5:
			this->curl_setopt(CURLOPT_PROXYTYPE,CURLPROXY_SOCKS5);
		break;
	}
}
CURLcode Curl::curl_perform(){
	return curl_easy_perform(this->curl);
}
CURLcode Curl::post(const char * url, const char * content, const char *cookie){
	this->headerInfo.clear();
	this->responseInfo.clear();
	if(strstr(url,"https://")){
		this->curl_setopt(CURLOPT_SSL_VERIFYPEER,0L);
		this->curl_setopt(CURLOPT_SSL_VERIFYHOST,0L);
	}
	if(cookie != NULL){
		this->curl_setopt(CURLOPT_COOKIE, cookie);
	}
	this->curl_setopt(CURLOPT_URL, url);
	this->curl_setopt(CURLOPT_POST,1L);
	this->curl_setopt(CURLOPT_POSTFIELDS, content);
	return this->curl_perform();
} 
CURLcode Curl::get(const char *url, const char *cookie){
	this->headerInfo.clear();
	this->responseInfo.clear();

	if(cookie != NULL){
		this->curl_setopt(CURLOPT_COOKIE, cookie);
	}

	if(strstr(url,"https://")){
		this->curl_setopt(CURLOPT_SSL_VERIFYPEER,0L);
		this->curl_setopt(CURLOPT_SSL_VERIFYHOST,0L);
	}
	this->curl_setopt(CURLOPT_URL, url);
	return this->curl_perform();
}
size_t Curl::header_callback( void *ptr, size_t size, size_t nmemb, std::string *userdata){
	userdata->append((const char *)ptr, size * nmemb);

	return size*nmemb;
}
size_t Curl::write_callback( void *ptr, size_t size, size_t nmemb, std::string *response){
	response->append((const char *)ptr, size * nmemb);

	return size*nmemb;
}
const char * Curl::response(){
	return this->responseInfo.c_str();
}
long Curl::status(){
	CURLcode r = curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &this->statusCode);
  //cout<<r<<endl;
	return statusCode;
}
const char * Curl::header(const char * field){
	std::string ret;
	size_t pos = this->headerInfo.find(field);
	if(pos != string::npos){
		size_t end = this->headerInfo.find("\r\n", pos); 
		ret = this->headerInfo.substr(pos + strlen(field), end - pos - strlen(field));
		pos = ret.find_first_not_of(" ");
		ret = ret.substr(ret.find_first_not_of(" "), end);
		return ret.c_str();
	}
	return NULL;
}
const char * Curl::header(){
	return this->headerInfo.c_str();
}
struct curl_slist * Curl::header(const char * header,struct curl_slist *list){
	list = curl_slist_append(list, header);
	this->curl_setopt(CURLOPT_HTTPHEADER, list);
	return list;
}
