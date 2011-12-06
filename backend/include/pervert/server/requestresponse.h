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
	void setMetadata(Layer* layer, Metadata* value);
	Metadata* getMetadata(Layer* layer);
	void writeFirstHeader(int code, char* str);
	void writeHeader(char* key, char* value);
	void write();
	void close();
private:
	int mg_success;
	ResponseStatus _rs;
	friend class Server;
	Response(struct mg_connection *c);
	map<Layer*,Metadata*> _metadata;
};

	
} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_REQUEST_H_ */ 