#include "VSHelper.h"
#include <json/json.h>
#include <json/json_object_private.h>

VSHelper::VSHelper(Curl &curl){
	this->userAgent = &curl;
}
VSHelper::~VSHelper(){
}
vector<std::string> VSHelper::getResultUrls(const char *html){
	using namespace htmlcxx;
	vector<std::string> vec;
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(html);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	for(; it != end; it++){
		if(strcasecmp(it->tagName().c_str(), "table") == 0){
			it->parseAttributes();
			if(strcasecmp(it->attribute("id").second.c_str(), "t2") == 0){
				tree<HTML::Node>::iterator tableIt = it.begin();
				tree<HTML::Node>::iterator tableEnd = it.end();
				for(int i = 1; tableIt != tableEnd; tableIt++){
					if(tableIt->isTag() && tableIt->tagName().find("tr") != string::npos){
						if(i > 1){
							tree<HTML::Node>::iterator trIt = tableIt.begin();
							tree<HTML::Node>::iterator trEnd = tableIt.end();
							for(int j = 1; trIt != trEnd; trIt++){
								if(trIt->tagName().find("td") != string::npos){
									if(j == 2){
										trIt++;
										trIt->parseAttributes();
										vec.push_back(trIt->attribute("href").second);
									}
									j++;
								}
							}
						}
						i++;
					}
				}
			}else{
        vec.clear();
      }
		}
	}
	return vec;
}
bool VSHelper::query(const char *md5){
	using namespace std;
	string url("http://md5.virscan.org/");
	userAgent->get(url.c_str());
	const char *cookie = userAgent->header("Set-Cookie:");
	url.clear();
	url.append("http://md5.virscan.org/");
	url.append(md5);
	userAgent->get(url.c_str(), cookie);
	vector<std::string> vec = getResultUrls(userAgent->response());
	if(vec.empty()){
    //cout<<"VSHelper:"<<userAgent->status()<<":"<<userAgent->response();
		return false;
  }
	userAgent->get(vec[0].c_str());
	using namespace htmlcxx;
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(userAgent->response());
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	for(; it != end; it++){
		if(strcasecmp(it->tagName().c_str(), "table") == 0){
			it->parseAttributes();
			if(strcasecmp(it->attribute("id").second.c_str(), "t2") == 0){
				tree<HTML::Node>::iterator tableIt = it.begin();
				tree<HTML::Node>::iterator tableEnd = it.end();
				json_object *array = json_object_new_array();
				for(int i = 0; tableIt != tableEnd; tableIt++){
					json_object *item;
					if(!tableIt->isTag() && !tableIt->isComment()){
						string result = tableIt->text();
						size_t pos = result.find_first_not_of("\t\n\r ");
						if(pos != string::npos){
							if(result.find("Scanner") != string::npos || result.find("Engine Ver") != string::npos || result.find("Sig Ver") != string::npos || result.find("Sig Date") != string::npos || result.find("Scan result") != string::npos || result.find("Time") != string::npos){
								continue;
							}
							while(result.find("&nbsp;") != string::npos)
								result.replace(result.find("&nbsp;"), 6, " ");
							if(i == 5){
								//cout<<endl;
								i = 0;
								json_object_array_add(array, item);
								result.clear();
							}else{
								switch(i){
									case 0:
										item = json_object_new_object();
										json_object_object_add(item, "scanner", json_object_new_string(result.c_str()));
									break;
									case 3:
										json_object_object_add(item, "scandate", json_object_new_string(result.c_str()));
									break;
									case 4:
										json_object_object_add(item, "report", json_object_new_string(result.c_str()));
									break;
								}
								i++;
							}
						}
					}
				}
				cout<<"VirScan\t"<<json_object_to_json_string(array)<<endl;
				json_object_put(array);
			}
		}
	}
	return true;
}
