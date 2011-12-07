//
//  layer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_SERVER_QUERY_H_
#define PERVERT_SERVER_QUERY_H_

#include <iostream>
#include <fstream>

#include "pervert/server/server.h"

namespace PerVERT {
namespace Server {
	
using namespace Utils;

struct QueryData : public Metadata {
	bool exists(string value);
	string operator[](string value);
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

	
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_SERVER_QUERY_H_ */ 