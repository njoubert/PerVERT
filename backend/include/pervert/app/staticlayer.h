//
//  staticlayer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_APP_STATICLAYER_H_
#define PERVERT_APP_STATICLAYER_H_

#include <iostream>
#include <fstream>

#include "pervert/server/server.h"

namespace PerVERT {
namespace App {
	
using namespace Utils;

class StaticLayer : public Server::Layer {
public:
	StaticLayer(char* httproot);
	void handle(Server::Request* req, Server::Response* res);
	char* name();
private:
	char* _httproot;
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_STATICLAYER_H_ */ 