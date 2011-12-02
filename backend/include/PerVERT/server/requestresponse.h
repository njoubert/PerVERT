//
//  request
//
//  Created by Niels Joubert on 2011-12-01 !!!
//

#ifndef PERVERT_SERVER_REQUESTRESPONSE_H_
#define PERVERT_SERVER_REQUESTRESPONSE_H_

#include "pervert/server/globals.h"

namespace PerVERT {
namespace Server {
	
enum ResponseStatus { NONE, FIRSTHEADER, HEADERS, DATA };
	
using namespace Utils;

struct Request {
	const struct mg_request_info *request_info;
private:
	friend class Server;
	unsigned int currentLayer;
	Request(const struct mg_request_info *req) { currentLayer = 0; request_info = req; }
};

struct Response {
	struct mg_connection *conn;
	void writeFirstHeader(int code, char* str);
	void writeHeader(char* key, char* value);
	void write();
private:
	int mg_success;
	ResponseStatus _rs;
	friend class Server;
	Response(struct mg_connection *c) { conn = c; mg_success = 1; _rs = NONE; }
};

	
} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_REQUEST_H_ */ 