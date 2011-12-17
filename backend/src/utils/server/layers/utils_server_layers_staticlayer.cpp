#include "utils/server/layers/staticlayer.h"

#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Utils {
namespace Server {
namespace Layers {


StaticLayer::StaticLayer(const char* httproot, unsigned int maxfilesize) {
	#define MAXPATHLEN 1024
	char temp[MAXPATHLEN];
	
	getcwd(temp, MAXPATHLEN); 
	string cwd = string(temp);
	#undef MAXPATHLEN
	
	_httproot = cwd.append(string(httproot));
	_maxfilesize = maxfilesize;
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
			return writeStatusAndEnd(req,res,404);
		}
		
		struct stat status;
		stat(f.c_str(), &status);
		
		//pull file from disk
		if (status.st_size > _maxfilesize) {
			writeStatusAndEnd(req,res,413);
		} else {
			char* filedata = new char[status.st_size];
			FILE* fd = fopen(f.c_str(),"r");
			if (fd == NULL) {
				return writeStatusAndEnd(req,res,500);
			} else {
				size_t readstatus = fread(filedata,sizeof(char),status.st_size,fd);
				fclose(fd);
				if (readstatus != status.st_size) {
					writeStatusAndEnd(req,res,500);
				} else {
					write200Response(req,res,UNKNOWN,filedata, readstatus);
				}
			}
			delete filedata;
		}
		
	} else {
		return next(req,res);
	}
	
}

const char* StaticLayer::name() {
	return "StaticLayer";
}

} /* namespace Layer */
} /* namespace Server */
} /* namespace PerVERT */
