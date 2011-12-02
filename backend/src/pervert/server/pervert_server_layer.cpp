#include "pervert/server/layer.h"
#include "pervert/server/server.h"

namespace PerVERT {
namespace Server {

void Layer::next(Request* req, Response* res) {
	_server->next(req, res);
}

void Layer::setServer(Server* server) {
	_server = server;
}

} /* namespace Server */
} /* namespace PerVERT */
