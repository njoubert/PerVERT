#include "utils/server/layer.h"
#include "utils/server/server.h"

namespace Utils {
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
} /* namespace Utils */
