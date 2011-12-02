#include "pervert/app/pervertlayer.h"

namespace PerVERT {
namespace App {

PervertLayer::PervertLayer() {

}


void PervertLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	
	
	
	next(req,res);
}

char* PervertLayer::name() {
	return "PervertLayer";
}

} /* namespace Server */
} /* namespace PerVERT */
