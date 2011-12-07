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
#include "pervert/server/httplayer.h"

namespace PerVERT {
namespace Server {
	
using namespace Utils;

class StaticLayer : public HTTPLayer {
public:
	StaticLayer(const char* httproot, unsigned int maxfilesize);
	void handle(Request* req, Response* res);
	const char* name();
private:
	string _httproot;
	unsigned int _maxfilesize;
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_STATICLAYER_H_ */ 