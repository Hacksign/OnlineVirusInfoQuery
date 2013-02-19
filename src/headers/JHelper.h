#ifndef _JHELERP_H_
#define _JHELERP_H_
#include <iostream>
#include <htmlcxx/html/ParserDom.h>
#include <vector>
#endif

#include "Curl.h"

class JHelper{
	public:
		JHelper(Curl &);
		bool query(const char *);
	  const char * setProxyHost();
		~JHelper();
	private:
		Curl *userAgent;

	private:
		string trim(string &);
};
