//
//  staticlayer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_LAYERS_STATICLAYER_H_
#define _UTILS_SERVER_LAYERS_STATICLAYER_H_

#include <iostream>
#include <fstream>

#include "utils/server/layers/httplayer.h"

namespace Utils {
namespace Server {
namespace Layers {

class StaticLayer : public HTTPLayer {
public:
	StaticLayer(const char* httproot, unsigned int maxfilesize);
	void handle(Request* req, Response* res);
	const char* name();
private:
	string _httproot;
	unsigned int _maxfilesize;
};


} /* namespace Layer */
} /* namespace Server */
} /* namespace Utils */


#endif /* _UTILS_SERVER_LAYERS_STATICLAYER_H_ */ 