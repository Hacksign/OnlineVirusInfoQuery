#ifndef _VTHELPER_H_
#define _VTHELPER_H_
#include <curl/curl.h>
#include <iostream>
#include <htmlcxx/html/ParserDom.h>
#endif

template <class T>
class VTHelper{
	public:
		VTHelper();
		~VTHelper();
		bool query(const char *);

	private:
		const char *getToken(const char *);
		std::string getResult(const char *);
		bool checkError(const char *);
		char * trim(char *);
	private:
		T *userAgent;
};
