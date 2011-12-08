//
//  jsonlayer
//
//  Created by Niels Joubert on 2011-12-07.
//

#ifndef PERVERT_APP_JSONLAYER_H_
#define PERVERT_APP_JSONLAYER_H_

#include <iostream>
#include <fstream>
#include "json/json.h"

#include "pervert/server/httplayer.h"

namespace PerVERT {
namespace Server {
	
using namespace Utils;
using ::Json::Value;
using ::Json::Reader;
using ::Json::Writer;

/* This is an intermediate layer object that provides an API to return JSON data. */
class JSONLayer : public HTTPLayer {
public:	

	virtual void handle(Request* req, Response* res) = 0;
	virtual const char* name() = 0;

protected:
	
	void writeJSONResponse(Request* req, Response* res, const Value& value);

private:

};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_JSONLAYER_H_ */ 