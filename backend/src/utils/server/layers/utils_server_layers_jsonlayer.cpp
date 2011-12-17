#include "utils/server/layers/jsonlayer.h"

#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Utils {
namespace Server {
namespace Layers {

using namespace ::Json;

void JSONLayer::writeJSONResponse(Request* req, Response* res, const Value& value) {
	StyledWriter writer; 	//todo(njoubert): Replace with FastWriter for production
	string out = writer.write(value);
	write200Response(req, res, APP_JSON, out.c_str(), out.length());
}
	
} /* namespace Layer */
} /* namespace Server */
} /* namespace PerVERT */

