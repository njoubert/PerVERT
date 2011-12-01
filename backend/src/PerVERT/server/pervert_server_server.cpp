#include "pervert/server/server.h"


namespace PerVERT {
namespace Server {

static void *event_handler(enum mg_event event,
                           struct mg_connection *conn,
                           const struct mg_request_info *request_info) {

	
	Server &server = Server::Instance();
	return (void*) server.handleRequest(event, conn, request_info);
}

static const char *options[] = {
  "document_root", "html",
  "listening_ports", "8083",
  "num_threads", "5",
  NULL
};

Server::Server() : _log(GETLOG("SERVER")) {
	ctx = NULL;
}

Server::~Server() {
	if (ctx != NULL) {
		_log.log(LOG_INFO, "Stopping mongoose server...\n");
		mg_stop(ctx);
	}
	
}

void Server::start() {
	//initializes Mongoose
	ctx = mg_start(&event_handler, NULL, options);
	if (ctx == NULL)
		throw 42;
	
	_log.log(LOG_STATUS,"PerVERT Server started on port %s.\n", mg_get_option(ctx, "listening_ports"));
}	

// handle the request, return 0 if everything is OK.
int Server::handleRequest(enum mg_event event,
                          struct mg_connection *conn,
                          const struct mg_request_info *request_info) {
	if (_log.getLevel() == LOG_DEBUG)
		printRequestInfo(request_info);
	int processed = 0;
	switch (event) {
			case MG_NEW_REQUEST:    // New HTTP request has arrived from the client
				//processed = (_requestHandler.handle(conn, request_info) == true) ? 1 : 0;
				break;
			case MG_HTTP_ERROR :    // HTTP error must be returned to the client
				_log.log(LOG_WARN, "Mongoose HTTP error.\n");
				break;
			case MG_EVENT_LOG  :    // Mongoose logs an event, request_info.log_message
				_log.log(LOG_WARN, "Mongoose Event Logged.\n");
				break;
			case MG_INIT_SSL   :    // Mongoose initializes SSL. Instead of mg_connection *,
				_log.log(LOG_WARN, "SSL Not Supported.\n");
				break;
	}
	return processed;
}

void Server::printRequestInfo(const struct mg_request_info *request_info) {
	_log.log(LOG_DEBUG, "request_method: %s\n", request_info->request_method);
	_log.log(LOG_DEBUG, "uri           : %s\n", request_info->uri );
	_log.log(LOG_DEBUG, "http_version  : %s\n", request_info->http_version);
	_log.log(LOG_DEBUG, "query_string  : %s\n", request_info->query_string);
	_log.log(LOG_DEBUG, "remote_user   : %s\n", request_info->remote_user);
	_log.log(LOG_DEBUG, "log_message   : %s\n", request_info->log_message);
	_log.log(LOG_DEBUG, "remote_ip     : %ld\n",request_info->remote_ip);
	_log.log(LOG_DEBUG, "rempote_port  : %d\n", request_info->remote_port);
	_log.log(LOG_DEBUG, "status_code   : %d\n", request_info->status_code);
	_log.log(LOG_DEBUG, "is_ssl        : %d\n", request_info->is_ssl);
	_log.log(LOG_DEBUG, "num_headers   : %d\n", request_info->num_headers);
	for (int i = 0; i < request_info->num_headers; i++) {
		_log.log(LOG_DEBUG, " header : [%s] = %s\n", request_info->http_headers[i].name, request_info->http_headers[i].value);
	}
	_log.log_cr(LOG_DEBUG);
}

} /* namespace Server */
} /* namespace PerVERT */
