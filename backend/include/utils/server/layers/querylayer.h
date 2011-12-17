//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef _UTILS_SERVER_LAYERS_QUERYLAYER_H_
#define _UTILS_SERVER_LAYERS_QUERYLAYER_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "utils/server/layer.h"

namespace Utils {
namespace Server {
namespace Layers {

struct QueryData : public Metadata {
	bool exists(string value);
	string get(string value);
private:
	map<string, string> _data;
	friend class QueryLayer;
};

class QueryLayer : public Layer {
public:
	QueryLayer();
	const char* name();

	void handle(Request* req, Response* res);

	void afterwards(Request* req, Response* res);
	
private:
	Log& _log;

};


} /* namespace Layer */
} /* namespace Server */
} /* namespace Utils */



#endif /* _UTILS_SERVER_LAYERS_QUERYLAYER_H_ */ 