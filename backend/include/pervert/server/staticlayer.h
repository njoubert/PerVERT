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
namespace Server {
	
using namespace Utils;

class StaticLayer : public Layer {
public:
	StaticLayer(char* httproot);
	void handle(Request* req, Response* res);
	char* name();
private:
	char* _httproot;
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_STATICLAYER_H_ */ 