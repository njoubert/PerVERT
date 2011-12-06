#include "pervert/app/pervertlayer.h"
#include "pervert/server/querylayer.h"

namespace PerVERT {
namespace App {

PervertLayer::PervertLayer()  : _log(GETLOG("PERVERT")){

}


void PervertLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->uri, "/data") != 0) {
		return next(req,res);
	}
	_log.log(LOG_INFO,"Data request received...");
	
	Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL) {
		writeStatus(req,res,500);
	} else {
		next(req,res);
	}
	
}

char* PervertLayer::name() {
	return "PervertLayer";
}

} /* namespace Server */
} /* namespace PerVERT */



