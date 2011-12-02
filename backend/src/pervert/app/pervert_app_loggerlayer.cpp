#include "pervert/app/loggerlayer.h"

namespace PerVERT {
namespace App {

void LoggerLayer::handle(Server::Request* req, Server::Response* res) {
	
	
	next(req,res);
}

char* LoggerLayer::name() {
	return "LoggerLayer";
}

} /* namespace Server */
} /* namespace PerVERT */
