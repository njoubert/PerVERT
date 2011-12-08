#include "pervert/app/pervertlayer.h"
#include "pervert/server/querylayer.h"

namespace PerVERT {
namespace App {

PervertLayer::PervertLayer() : _log(GETLOG("PERVERT")) {
	
	
}

void PervertLayer::ping(Server::Request* req, Server::Response* res) {
	write200Response(req,res,TEXT_PLAIN,"pong",4);
}

void PervertLayer::f_status(Server::Request* req, Server::Response* res) {
	Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL ||
		!query->exists("exec")) {
		return writeStatusAndEnd(req,res,500);
	} else {
		string exec = query->get("exec");
		if (_dms.count(exec) < 1) {
			return write200Response(req,res,TEXT_PLAIN,"The given executable is not known to Pervert.",45);		
		} else {
			bool busy = _dms[exec]->status();
			if (busy) {
				return write200Response(req,res,TEXT_PLAIN,"BUSY",4);				
			} else {
				return write200Response(req,res,TEXT_PLAIN,"GOOD",4);

			}
		}
		
	}
}

void PervertLayer::pp_update(Server::Request* req, Server::Response* res) {
			
	Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL ||
		!query->exists("exec") ||
		!query->exists("logs")) {
		return writeStatusAndEnd(req,res,500);
	} else {
		
		DataManager* dm;
		string exec = query->get("exec");
		if (_dms.count(exec) < 1) {
			_log.log(LOG_INFO, "Creating new DataManager for %s\n", exec.c_str());
			dm = new DataManager(exec);
			_dms[exec] = dm;
		} else {
			_log.log(LOG_INFO, "Found DataManager for %s\n", exec.c_str());
			dm = _dms[exec];
		}
		
		int success = dm->update(query->get("logs"));
		
		if (success == 0) {
			_log.log(LOG_STATUS, "Successfully handled /pp/update query\n");
			return writeStatusAndEnd(req,res,200);
		} else {
			_log.log(LOG_WARN, "Failed to handle /pp/update query\n");
			return writeStatusAndEnd(req,res,500);
		}
		
	}
	
}

void PervertLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->uri, "/ping") == 0) {
		return ping(req,res);
	} else if (strcmp(request_info->uri, "/pp/update") == 0) {
		return pp_update(req,res);
	} else if (strcmp(request_info->uri, "/f/status") == 0) {
		return f_status(req,res);
	} else {
		next(req,res);
	}
	
}

const char* PervertLayer::name() {
	return "PervertLayer";
}

} /* namespace Server */
} /* namespace PerVERT */



