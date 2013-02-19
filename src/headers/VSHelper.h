#ifndef _VSHELERP_H_
#define _VSHELERP_H_
#include <iostream>
#include <htmlcxx/html/ParserDom.h>
#include <vector>
#endif

#include "Curl.h"

class VSHelper{
	public:
		VSHelper(Curl &);
		bool query(const char *);
		vector<std::string> getResultUrls(const char *);
		~VSHelper();
	private:
		Curl *userAgent;

	private:
		string trim(string &);
};
