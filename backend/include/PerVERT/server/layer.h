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
	virtual void handle(Request* req, Response* res) = 0;
	virtual char* name() = 0;
	
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