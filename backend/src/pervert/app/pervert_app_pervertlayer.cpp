#include "pervert/app/pervertlayer.h"
#include "pervert/server/querylayer.h"
#include "pervert/app/datamanager.h"

namespace PerVERT {
namespace App {

PervertLayer::PervertLayer()  : _log(GETLOG("PERVERT")){
	
	
}

void PervertLayer::ping(Server::Request* req, Server::Response* res) {
	writeOKResponseWithContentLength(req,res,"pong",4);
}

void PervertLayer::f_status(Server::Request* req, Server::Response* res) {
	writeOKResponseWithContentLength(req,res,"UP",2);
}

void PervertLayer::pp_update(Server::Request* req, Server::Response* res) {
			
	Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL) {
		return writeStatusAndEnd(req,res,500);
	}
	
}

void PervertLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->uri, "/ping") == 0) {
		return ping(req,res);
	} else if (strcmp(request_info->uri, "/pp/update") == 0) {
		return pp_update(req,res);
	} else if (strcmp(request_info->uri, "/f/status")) {
		return f_status(req,res);
	} else {
		next(req,res);
	}
	
}

const char* PervertLayer::name() {
	return "PervertLayer";
}

} /* namespace Server */
} /* namespace PerVERT */



