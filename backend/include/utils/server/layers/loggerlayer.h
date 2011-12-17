//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_LAYERS_LOGGERLAYER_H_
#define _UTILS_SERVER_LAYERS_LOGGERLAYER_H_

#include <iostream>
#include <fstream>

#include "utils/server/layer.h"

namespace Utils {
namespace Server {
namespace Layers {

enum LoggerLayerLevel { TINY, BIG };

class LoggerLayer : public Layer {
public:
	LoggerLayer(LoggerLayerLevel l, const char* file);
	const char* name();

	void handle(Request* req, Response* res);

	void afterwards(Request* req, Response* res);
	
private:
	std::ofstream _file;
	LoggerLayerLevel _l;
	
};


} /* namespace Layer */
} /* namespace Server */
} /* namespace Utils */



#endif /* _UTILS_SERVER_LAYERS_LOGGERLAYER_H_ */ 