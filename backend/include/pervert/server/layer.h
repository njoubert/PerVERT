//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_SERVER_LAYER_H_
#define PERVERT_SERVER_LAYER_H_

#include "pervert/server/globals.h"
#include "pervert/server/requestresponse.h"

namespace PerVERT {
namespace Server {
	
using namespace Utils;

class Server;

class Layer {
public:
	virtual void init();
	virtual char* name() = 0;

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
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_LAYER_H_ */ 