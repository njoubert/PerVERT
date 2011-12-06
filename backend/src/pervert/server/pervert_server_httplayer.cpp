#include "pervert/server/httplayer.h"
#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

namespace PerVERT {
namespace Server {

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
void HTTPLayer::writeStaticPage(Request* req, Response* res, char* data, size_t len) {
	
	static const char *ajax_reply_start =
		"HTTP/1.1 200 OK\r\n"
		"Cache: no-cache\r\n";
	mg_printf(res->conn, "%sContent-Length:%d\r\n\r\n", ajax_reply_start, len);
	res->write(data, len);
	mg_printf(res->conn, "\r\n");
	
}

} /* namespace Server */
} /* namespace PerVERT */
