#include "utils/server/layers/loggerlayer.h"

#include <arpa/inet.h>
#include <sys/time.h>

namespace Utils {
namespace Server {
namespace Layers {

struct LoggerMetadata : public Metadata {
	timeval starttime;
};

LoggerLayer::LoggerLayer(LoggerLayerLevel l, const char* file) {
	_l = l;
	_file.open(file, std::ios::out);
}


void LoggerLayer::handle(Request* req, Response* res) {
	LoggerMetadata* m = new LoggerMetadata(); //TODO: should be "current time"
	gettimeofday(&(m->starttime), 0);
	res->setMetadata("logger", m);
	next(req,res);
}

void LoggerLayer::afterwards(Request* req, Response* res) {
	LoggerMetadata* m = (LoggerMetadata*) res->getMetadata("logger");
	timeval currentTime;
	gettimeofday(&currentTime, 0);
	double duration = (double) (1000*(currentTime.tv_sec - m->starttime.tv_sec) + 1e-3
            * (currentTime.tv_usec - m->starttime.tv_usec));
	delete m;

	const struct mg_request_info *request_info = req->request_info;
	struct in_addr a;
	a.s_addr = htonl(request_info->remote_ip);
	if (_l == TINY) {
		_file << inet_ntoa(a);
		_file << " [HTTP " << request_info->http_version << " " << request_info->request_method << "] ";
		_file << request_info->uri;
		_file << " " << duration << " ms";
	} else if (_l == BIG) {	
		_file << "request_method: " << request_info->request_method;
		_file << "uri           : " << request_info->uri ;
		_file << "http_version  : " << request_info->http_version;
		_file << "query_string  : " << request_info->query_string;
		_file << "remote_user   : " << request_info->remote_user;
		_file << "log_message   : " << request_info->log_message;
		_file << "remote_ip     : " << request_info->remote_ip;
		_file << "remote_port   : " << request_info->remote_port;
		_file << "status_code   : " << request_info->status_code;
		_file << "is_ssl        : " << request_info->is_ssl;
		_file << "num_headers   : " << request_info->num_headers;
		_file << "duration      : " << duration << " ms";
		for (int i = 0; i < request_info->num_headers; i++) {
			_file << " header : [" <<  request_info->http_headers[i].name << "] = " << request_info->http_headers[i].value << "\n";
		}
	}
	_file << "\n";
	_file.flush();	
}

const char* LoggerLayer::name() {
	return "LoggerLayer";
}

} /* namespace Layer */
} /* namespace Server */
} /* namespace PerVERT */

