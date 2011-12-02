//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_APP_LOGGER_H_
#define PERVERT_APP_LOGGER_H_

#include "pervert/server/server.h"

namespace PerVERT {
namespace App {
	
using namespace Utils;

class LoggerLayer : public Server::Layer {
public:
	void handle(Server::Request* req, Server::Response* res);
	char* name();

};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_LAYER_H_ */ 