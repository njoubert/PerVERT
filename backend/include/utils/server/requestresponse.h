//
//  request
//
//  Created by Niels Joubert on 2011-12-01 !!!
//

#ifndef _UTILS_SERVER_REQUESTRESPONSE_H_
#define _UTILS_SERVER_REQUESTRESPONSE_H_

#include "utils/server/globals.h"

namespace Utils {
namespace Server {
	
enum ResponseStatus { NONE, FIRSTHEADER, HEADERS, DATA, CLOSED };

class Layer;

struct Metadata {
	//extend this inside your layers with your own custom data.
};
	
using namespace Utils;

struct Request {
	const struct mg_request_info *request_info;
private:
	friend class Server;
	unsigned int currentLayer;
	Request(const struct mg_request_info *req);
};

struct Response {
	struct mg_connection *conn;
	void setMetadata(string name, Metadata* value);
	Metadata* getMetadata(string name);
	void write(const char* data, size_t len);
	void printf(const char *fmt, ...);
	void close();
private:
	int mg_success;
	ResponseStatus _rs;
	friend class Server;
	Response(struct mg_connection *c);
	map<string,Metadata*> _metadata;
};

	
} /* namespace Server */
} /* namespace Utils */


#endif /* _UTILS_SERVER_REQUESTRESPONSE_H_ */ 