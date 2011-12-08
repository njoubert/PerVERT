#include "pervert/server/jsonlayer.h"
#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

namespace PerVERT {
namespace Server {
	
using namespace ::Json;

void JSONLayer::writeJSONResponse(Request* req, Response* res, const Value& value) {
	StyledWriter writer; 	//todo(njoubert): Replace with FastWriter for production
	string out = writer.write(value);
	write200Response(req, res, APP_JSON, out.c_str(), out.length());
}
	
} /* namespace Server */
} /* namespace PerVERT */
