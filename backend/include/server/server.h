//
//  server
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef SERVER_H_
#define SERVER_H_

#include "server/globals.h"

namespace Backend {
namespace Server {
	
	
class Server {
public:
	//using Meyers singleton pattern
	static Server& Instance() {
		static Server theSingleton;
		return theSingleton;
	}
	
	void start();
	int handleRequest(enum mg_event event,
                           struct mg_connection *conn,
                           const struct mg_request_info *request_info);
private:
	struct mg_context *ctx;
	Log& _log;
	void printRequestInfo(const struct mg_request_info *request_info);
	
	//singleton
	Server();
	explicit Server(Server const&);
	Server& operator=(Server const&);
	~Server();
};

	
} /* namespace Server */
} /* namespace Backend */


#endif /* SERVER_H_ */ 