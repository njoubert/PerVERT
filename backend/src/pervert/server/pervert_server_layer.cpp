#include "pervert/server/layer.h"
#include "pervert/server/server.h"

namespace PerVERT {
namespace Server {

void Layer::init() {
	
}

void Layer::afterwards(Request* req, Response* res) {
	
}

void Layer::next(Request* req, Response* res) {
	_server->next(req, res);
}

void Layer::setServer(Server* server) {
	_server = server;
}

} /* namespace Server */
} /* namespace PerVERT */
