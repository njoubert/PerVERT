//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_APP_LOGGER_H_
#define PERVERT_APP_LOGGER_H_

#include <iostream>
#include <fstream>

#include "pervert/server/server.h"

namespace PerVERT {
namespace App {
	
using namespace Utils;

enum LoggerLayerLevel { TINY, BIG };

class LoggerLayer : public Server::Layer {
public:
	LoggerLayer(LoggerLayerLevel l, char* file);
	void handle(Server::Request* req, Server::Response* res);
	char* name();
private:
	std::ofstream _file;
	LoggerLayerLevel _l;
	
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_LAYER_H_ */ 