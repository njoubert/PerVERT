#include "utils/server/layers/httplayer.h"

#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

namespace Utils {
namespace Server {
namespace Layers {

static const char* content_type_to_string[] = {
	"unknown",
	"application/json",
	"application/javascript",
	"text/plain",
	"text/html",
	"text/css"
};

void HTTPLayer::writeHeadersEnd(Request* req, Response* res) {
	res->write("\r\n", 2);
}

void HTTPLayer::writeStatus(Request* req, Response* res, int code) {
	switch(code) {
		case 200: //OK
		res->write("HTTP/1.1 200 OK\r\n", 17);
		break;

		case 404: //Request Entity Too Large
		res->write("HTTP/1.1 404 Not Found\r\n", 24);
		break;
				
		case 413: //Request Entity Too Large
		res->write("HTTP/1.1 413 Request Entity Too Large\r\n", 39);
		break;
		
		case 500: //Internal Server Error
		res->write("HTTP/1.1 500 Internal Server Error\r\n", 36);
		break;
		
		
		default:
		break;
	}
}
void HTTPLayer::writeStatusAndEnd(Request* req, Response* res, int code) {
	writeStatus(req,res,code);
	writeHeadersEnd(req,res);
}
void HTTPLayer::write200Response(Request* req, Response* res, CONTENT_TYPE ct, const char* data, size_t len) {
	writeStatus(req,res,200);
	static const char *headers = 
		"Cache: no-cache\r\n"
		"Content-Length: %d\r\n"
		"%s%s%s"
		"\r\n";
	char buffer[1024];
	if (ct == UNKNOWN) {
		sprintf(buffer, headers, len, "", "", "");		
	} else {
		sprintf(buffer, headers, len, "Content-Type: ", content_type_to_string[ct], "\r\n");
	}
	res->write(buffer, strlen(buffer));
	res->write(data, len);
	res->write("\r\n", 2);
}


} /* namespace Layer */
} /* namespace Server */
} /* namespace Util */
