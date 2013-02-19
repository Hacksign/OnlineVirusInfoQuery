#include "Curl.h"
#include "VTHelper.cpp"
#include "VSHelper.h"
#include "JHelper.h"
#include <iostream>

using namespace std;
void usage(char **argv){
	cout<<"usage:\n\t"<<argv[0]<<" md5"<<endl;
}
int main(int argc, char **argv){
	if(argc != 2){
		usage(argv);
		return 0;
	}
	VTHelper<Curl> vt;
	if(!vt.query(argv[1])){
		Curl curl;
		VSHelper vs(curl);
		if(!vs.query(argv[1])){
			JHelper j(curl);
			if(!j.query(argv[1])){
				cout<<">File Not Found!<"<<endl;
			}
		}
	}

	return 0;
}
