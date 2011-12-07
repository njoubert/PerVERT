#include "pervert/server/requestresponse.h"

namespace PerVERT {
namespace Server {

/***************************************************************************
 Request
 ***************************************************************************/


Request::Request(const struct mg_request_info *req) { currentLayer = 0; request_info = req; }







/***************************************************************************
  Response
 ***************************************************************************/


Response::Response(struct mg_connection *c) { conn = c; mg_success = 1; _rs = NONE; }

void Response::write(const char* data, size_t len) {
	mg_write(conn, data, len);
}

void Response::setMetadata(string name, Metadata* value) {
	_metadata[name] = value;
}

Metadata* Response::getMetadata(string name) {
	if (_metadata.count(name) > 0) {
		return _metadata[name];
	} else {
		return NULL;
	}
}


} /* namespace Server */
} /* namespace PerVERT */
