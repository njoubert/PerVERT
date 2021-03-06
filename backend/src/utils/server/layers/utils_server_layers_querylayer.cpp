#include "utils/server/layers/querylayer.h"

namespace Utils {
namespace Server {
namespace Layers {
	
bool QueryData::exists(string value) {
	return _data.count(value) > 0;
}

string QueryData::get(string value) {
	if (exists(value)) {
		return _data[value];
	} else {
		return "";
	}
}


QueryLayer::QueryLayer() : _log(GETLOG("QUERY")) { }

void QueryLayer::handle(Request* req, Response* res) {
	//We just parse the querystring
	if (req->request_info->query_string) {
		QueryData* qd = new QueryData();
		
		
		string all = string(req->request_info->query_string);
		vector<string> entries;
		if (all.find("&",0) != string::npos) {
			int start=0, end=0;
			while ((end = all.find("&", start)) != string::npos) {
				entries.push_back(all.substr(start, end-start));
				start = end+1;
			}
			if ((all.length()-start) > 0) {
				entries.push_back(all.substr(start, all.length()-start));				
			}
		} else {
			entries.push_back(all);
		}
		
		for (unsigned int i = 0; i < entries.size(); i++) {
			int l = 0;
			if ((l = entries[i].find("=")) != string::npos) {
				string key = entries[i].substr(0, l);
				string value = entries[i].substr(l+1, entries[i].length() - l-1);
				_log.log(LOG_DEBUG, "Parsed query {%s: %s}\n", key.c_str(), value.c_str());
				qd->_data[key] = value;
			} else {
				delete qd;
				return next(req,res);
			}
		}	
		res->setMetadata("query", qd);
	}
	next(req,res);
}

void QueryLayer::afterwards(Request* req, Response* res) {
	QueryData* qd = (QueryData*) res->getMetadata("query");
	if (qd != NULL) {
		delete qd;
	}
}

const char* QueryLayer::name() {
	return "QueryLayer";
}

} /* namespace Layer */
} /* namespace Server */
} /* namespace PerVERT */

