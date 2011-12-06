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

void Response::writeFirstHeader(int code, char* str) {
	
}
void Response::writeHeader(char* key, char* value) {
	
}
void Response::write() {
	
}

void Response::setMetadata(Layer* layer, Metadata* value) {
	_metadata[layer] = value;
}

Metadata* Response::getMetadata(Layer* layer) {
	return _metadata[layer];
}


} /* namespace Server */
} /* namespace PerVERT */
