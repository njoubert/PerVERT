//
//  server
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_SERVER_H_
#define _UTILS_SERVER_SERVER_H_

#include "utils/server/globals.h"
#include "utils/server/layer.h"
#include <vector>

namespace Utils {
namespace Server {
	
class Server {
public:
	//using Meyers singleton pattern
	static Server& Instance() {
		static Server theSingleton;
		return theSingleton;
	}
	
	void start();
	void registerLayer(Layer* layer);
	int handleRequest(enum mg_event event,
                           struct mg_connection *conn,
                           const struct mg_request_info *request_info);
	
private:
	struct mg_context *ctx;
	Log& _log;
	
	//singleton
	Server();
	explicit Server(Server const&);
	Server& operator=(Server const&);
	~Server();

	//layer management
	friend class Layer;
	void next(Request* req, Response* res);
	void afterwards(Request* req, Response* res);
	std::vector<Layer*> _layers;
	
};

	
} /* namespace Server */
} /* namespace Utils */


#endif /* _UTILS_SERVER_SERVER_H_ */ 