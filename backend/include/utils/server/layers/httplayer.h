//
//  httplayer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_LAYERS_HTTPLAYER_H_
#define _UTILS_SERVER_LAYERS_HTTPLAYER_H_

#include <iostream>
#include <fstream>

#include "utils/server/layer.h"

namespace Utils {
namespace Server {
namespace Layers {
	
/* This is an intermediate layer object that provides an API to return HTTP data. */
class HTTPLayer : public Layer {
public:	
	enum CONTENT_TYPE {
		UNKNOWN = 0,
		APP_JSON = 1,
		APP_JAVASCRIPT = 2,
		TEXT_PLAIN = 3,
		TEXT_HTML = 4,
		TEXT_CSS = 5
	};

	virtual void handle(Request* req, Response* res) = 0;
	virtual const char* name() = 0;

protected:
	void writeHeadersEnd(Request* req, Response* res);
	void writeStatus(Request* req, Response* res, int code);
	void writeStatusAndEnd(Request* req, Response* res, int code);
	void write200Response(Request* req, Response* res, CONTENT_TYPE ct, const char* data, size_t len);

private:

};

} /* namespace Layer */
} /* namespace Server */
} /* namespace Utils */


#endif /* _UTILS_SERVER_LAYERS_HTTPLAYER_H_ */ 