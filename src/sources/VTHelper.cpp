#include "VTHelper.h"
#include <string.h>
#include <json/json.h>
#include <json/json_object_private.h>

using namespace std;
template <class Curl> VTHelper<Curl>::VTHelper(){
	this->userAgent = new Curl;
}
template <class Curl> VTHelper<Curl>::~VTHelper(){
	delete this->userAgent;
}
template <class T> bool VTHelper<T>::query(const char *md5){
	do{
		//get Cookie
    do{
    this->userAgent->get("https://www.virustotal.com");
    }while(userAgent->status() != 200);
    const char* cookieStr = this->userAgent->header("Set-Cookie:");
	 	size_t len = strlen(cookieStr);
		char *pCookie = new char[len + 1];
		memset(pCookie, 0x00, len + 1);
		memcpy(pCookie, cookieStr, len);
		//construct post data
		string content("csrfmiddlewaretoken=");
		content.append(this->getToken(pCookie));//parase token from cookie
		content.append("&query=");
		content.append(md5);
		struct curl_slist *list = NULL;
		this->userAgent->header("Referer:https://www.virustotal.com/", list);
		//do query
    do{
      this->userAgent->post("https://www.virustotal.com/search/", content.c_str(), pCookie);
    }while(userAgent->status() != 200);
		delete[] pCookie;
	}while(this->checkError(this->userAgent->response()));
  string tmp = getResult(this->userAgent->response());
	if(!tmp.empty()){
		cout<<"VirusTotal\t"<<tmp<<endl;
    return true;
  }else{
		//cout<<">File Not Found!<"<<endl;
    //cout<<"VTHelper:"<<userAgent->status()<<" "<<userAgent->header()<<" "<<userAgent->response();
		return false;
	}
  return false;
}
template <class T> const char * VTHelper<T>::getToken(const char *cookie){
  try{
    string ret(cookie);
    size_t start = ret.find_first_of("=") + 1;
    size_t end = ret.find_first_of(";");
    ret = ret.substr(start, end - start);

    return ret.c_str();
  }catch(exception &e){
    cout<<e.what()<<endl;
  }
}
template <class T> bool VTHelper<T>::checkError(const char *content){
  try{
    if(!content || (content && strlen(content) == 0) || userAgent->status() != 200){
      return true;
    }
    string html(content);
    using namespace htmlcxx;
    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(html);
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    for(; it != end; it++){
      if(!it->isTag() && !it->isComment()){
        string result = it->text();
        if(result.find("Ohch! Something went wrong.") != string::npos){
          cout<<"someting wrong"<<endl;
          return true;
        }else if(result.find("You have exceeded your request rate limit") != string::npos){
          cout<<"max limit"<<endl;
          return true;
        }
      }
    }
    return false;
  }catch(exception& e){
    cout<<e.what()<<endl;
  }
}
template <class T> string VTHelper<T>::getResult(const char *content){
  try{
    if(!content)
      return string("");
    string html(content);
    using namespace htmlcxx;
    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(html);
    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();
    for(; it != end; it++){
      if(strcasecmp(it->tagName().c_str(), "table") == 0){
        it->parseAttributes();
        if(strcasecmp(it->attribute("id").second.c_str(), "antivirus-results") == 0){
          tree<HTML::Node>::iterator tableIt = it.begin();
          tree<HTML::Node>::iterator tableEnd = it.end();
					json_object *array = json_object_new_array();
          for(int i = 0; tableIt != tableEnd; tableIt++){
            if(!tableIt->isTag() && !tableIt->isComment()){
							json_object *item;
              string result  = tableIt->text();
              size_t len = strlen(result.c_str()) + 1;
              char *a = new char[len];
              memset(a, 0x00, len);
              memcpy(a, result.c_str(), len);
              a = this->trim(a);
              if (a != NULL){
								if(strstr(a, "Antivirus") || strstr(a, "Result") || strstr(a, "Update")){
									continue;
								}
                if(i == 2){
                  i = 0;
									json_object_object_add(item, "scandate", json_object_new_string(result.c_str()));
									json_object_array_add(array, item);
                }else{
									switch(i){
										case 0:
											item = json_object_new_object();
											json_object_object_add(item, "scanner", json_object_new_string(result.c_str()));
										break;
										case 1:
											json_object_object_add(item, "report", json_object_new_string(result.c_str()));
										break;
									}
									i++;
                }
              }
              delete [] a;
            }
          }
          return string(json_object_to_json_string(array));
        }
      }else{
        continue;
      }
    }
    return string("");
  }catch(exception& e){
    cout<<e.what()<<endl;
  }
}
template <class T> char * VTHelper<T>::trim(char *str)
{
	char *p = str;
	while(*p == '\n' || *p == ' '){
		p++;
		if(*p == 0x00){
			return NULL;
		}
	}

	return p;
}
