#include "JHelper.h"
#include <json/json.h>
#include <json/json_object_private.h>

JHelper::JHelper(Curl &curl){
	this->userAgent = &curl;
}
JHelper::~JHelper(){
}

bool JHelper::query(const char *md5){
	int i = 0;
	string urlInfo("http://virusscan.jotti.org/nestor/getfileforhash.php?sessionid=session_legacy&output=json&hash=");
	string urlScan("http://virusscan.jotti.org/nestor/getscanprogress.php?sessionid=session_legacy&lang=en&output=json&scanid=");
	urlInfo.append(md5);
	do{
		i++;//try 10 times
		this->userAgent->get(urlInfo.c_str());
		if(this->userAgent->status() != 200){
			continue;
		}

		string tmp(this->userAgent->response());
		size_t jsonDataLen = tmp.find_last_of("}]") - tmp.find_first_of("[{") + 1;
		//cout<<tmp.substr(tmp.find_first_of("[{"),jsonDataLen).c_str()<<endl;
		json_object *scanId = json_tokener_parse(tmp.substr(tmp.find_first_of("[{"),jsonDataLen).c_str());
		switch(json_object_get_type(scanId)){
			case json_type_array:
			json_object *t;
				for(int j = 0; j < json_object_array_length(scanId); j++){
					t = json_object_array_get_idx(scanId, j);
					json_object_iter iter;
					json_object_object_foreachC(t, iter){
						if(strstr(iter.key, "id")){
							string id(json_object_to_json_string(iter.val));
							id.erase(id.find("\""),1);
							id.erase(id.rfind("\""),1);
							urlScan.append(id);
							json_object_put(scanId);
							json_object_put(t);
							goto c;
						}
					}
				}
			break;
			case json_type_object:
				i--;
				this->setProxyHost();
			 return false;
		  break;
		}
		break;
	}while( i != 10 );
c:
		i = 0;
		do{
			i++;
			this->userAgent->get(urlScan.c_str());
			if(this->userAgent->status() != 200)
				continue;	
			string tmp(this->userAgent->response());
			size_t jsonDataLen = tmp.find_last_of("}]") - tmp.find_first_of("[{") + 1;
			json_object *scanInfo = json_tokener_parse(tmp.substr(tmp.find_first_of("[{"),jsonDataLen).c_str());
			switch(json_object_get_type(scanInfo)){
				case json_type_object:
					json_object_iter iter;
					json_object_object_foreachC(scanInfo, iter){
						if(strstr(iter.key, "scanner")){
							json_object *array = json_object_new_array();
							for(int j = 0; j < json_object_array_length(iter.val); j++){
								json_object *item = json_object_new_object();
								json_object *t = json_object_array_get_idx(iter.val, j);
								json_object_object_foreach(t, key, val){
									if(strstr(key, "scannerid")){
										json_object_object_add(item,"scanner",val);
									}else if(strstr(key, "virusname")){
										json_object_object_add(item, "report", val);
									}else if(strstr(key, "lastupdate")){
										json_object_object_add(item, "scandate", val);
									}
								}
								json_object_array_add(array, item);
							}
							cout<<"Jotti\t"<<json_object_to_json_string(array)<<endl;
							json_object_put(array);
							json_object_put(scanInfo);
							return true;
						}
					}
					json_object_put(scanInfo);
				break;
			}
			break;
		}while( i != 10 );
	if(this->userAgent->status() != 200 && i == 10){
		return false;
	}
}
const char * JHelper::setProxyHost(){
	//{"type":"info","code":"RATELIMIT_EXCEEDED"}
	this->userAgent->proxy("190.0.57.98:8080");

	return NULL;
}
