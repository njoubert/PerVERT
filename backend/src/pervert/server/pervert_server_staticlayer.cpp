#include "pervert/server/staticlayer.h"
#include <unistd.h>

namespace PerVERT {
namespace Server {

StaticLayer::StaticLayer(char* httproot) {
	#define MAXPATHLEN 1024
	char temp[MAXPATHLEN];
	
	getcwd(temp, MAXPATHLEN); 
	string cwd = string(temp);
	#undef MAXPATHLEN
	
	_httproot = cwd.append(string(httproot));
}

void StaticLayer::handle(Request* req, Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->request_method,"GET") == 0) {
		
		//set up path
		string f = string(_httproot);
		string uri = string(request_info->uri);
		f.append(uri);
		if ((uri.at(uri.length() - 1)) == '/') {
			f.append("index.html");
		}
		
		//does this file exist?
		if (access(f.c_str(), R_OK) != 0) {
			return next(req,res);
		}
		
		
		//pull file from disk
		
		
		
		//set headers correctly
		
		//send file across wire
		
		
		
	} else {
		return next(req,res);
	}
	
}

char* StaticLayer::name() {
	return "StaticLayer";
}

} /* namespace Server */
} /* namespace PerVERT */
