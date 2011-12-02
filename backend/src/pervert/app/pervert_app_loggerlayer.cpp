#include "pervert/app/loggerlayer.h"

namespace PerVERT {
namespace App {

LoggerLayer::LoggerLayer(LoggerLayerLevel l, char* file) {
	_l = l;
	_file.open(file, std::ios::out);
}


void LoggerLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (_l == TINY) {
		_file << request_info->remote_ip << ":" << request_info->remote_port;
		_file << " [HTTP " << request_info->http_version << " " << request_info->request_method << "] ";
		_file << request_info->uri;
		printf("LOGGED\n");
	} else if (_l == BIG) {	
		_file << "request_method: " << request_info->request_method;
		_file << "uri           : " << request_info->uri ;
		_file << "http_version  : " << request_info->http_version;
		_file << "query_string  : " << request_info->query_string;
		_file << "remote_user   : " << request_info->remote_user;
		_file << "log_message   : " << request_info->log_message;
		_file << "remote_ip     : " << request_info->remote_ip;
		_file << "rempote_port  : " << request_info->remote_port;
		_file << "status_code   : " << request_info->status_code;
		_file << "is_ssl        : " << request_info->is_ssl;
		_file << "num_headers   : " << request_info->num_headers;
		for (int i = 0; i < request_info->num_headers; i++) {
			_file << " header : [" <<  request_info->http_headers[i].name << "] = " << request_info->http_headers[i].value << "\n";
		}
	}
	_file << "\n";
	_file.flush();
	next(req,res);
}

char* LoggerLayer::name() {
	return "LoggerLayer";
}

} /* namespace Server */
} /* namespace PerVERT */
