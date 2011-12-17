//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_LAYER_H_
#define _UTILS_SERVER_LAYER_H_

#include "utils/server/globals.h"
#include "utils/server/requestresponse.h"

namespace Utils {
namespace Server {
	
class Server;

class Layer {
public:
	virtual void init();
	virtual const char* name() = 0;

	//called by the server when this layer starts running.
	virtual void handle(Request* req, Response* res) = 0;
	
	//called by the server when all the layers finished running.
	//do not write to response, it's closed by now!
	virtual void afterwards(Request* req, Response* res);
	
protected:
	void next(Request* req, Response* res);

private:
	friend class Server;
	void setServer(Server* server);
	Server* _server;
};

} /* namespace Server */
} /* namespace Utils */


#endif /* _UTILS_SERVER_LAYER_H_ */ 