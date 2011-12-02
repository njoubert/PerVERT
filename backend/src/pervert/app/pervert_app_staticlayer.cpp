#include "pervert/app/staticlayer.h"

namespace PerVERT {
namespace App {

StaticLayer::StaticLayer(char* httproot) {
	_httproot = httproot;
}


void StaticLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->request_method,"GET") == 0) {
		
		//printf("uri: %s\n", request_info->uri);
		
	} else {
		next(req,res);
	}
	
}

char* StaticLayer::name() {
	return "StaticLayer";
}

} /* namespace Server */
} /* namespace PerVERT */
