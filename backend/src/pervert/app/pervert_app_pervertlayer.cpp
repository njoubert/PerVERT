#include "pervert/app/pervertlayer.h"
#include "pervert/server/querylayer.h"

#include <cstdlib>

namespace PerVERT {
namespace App {

PervertLayer::PervertLayer() : _log(GETLOG("PERVERT")) {
	_lastChangeID = 0;
}

void PervertLayer::logChange() {
	__sync_add_and_fetch(&_lastChangeID,1); //atomic add
}


void PervertLayer::ping(Server::Request* req, Server::Response* res) {
	write200Response(req,res,TEXT_PLAIN,"pong",4);
}

void PervertLayer::writePollResponse(Server::Request* req, Server::Response* res, const char* text) {
	char buffer[512];
	sprintf(buffer,"%d %s", _lastChangeID, text);
	write200Response(req,res,TEXT_PLAIN,buffer,strlen(buffer));
}
void PervertLayer::f_status(Server::Request* req, Server::Response* res) {
	Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL ||
		!query->exists("exec")) {
		return writeStatusAndEnd(req,res,500);
	} else {
		string exec = query->get("exec");
		if (_dms.count(exec) < 1) {
			return writePollResponse(req,res,"The given executable is not known to Pervert.");		
		} else {
			bool busy = _dms[exec]->status();
			if (busy) {
				return writePollResponse(req,res,"BUSY");				
			} else {
				return writePollResponse(req,res,"GOOD");

			}
		}
		
	}
}

void PervertLayer::pp_update(Server::Request* req, Server::Response* res) {
	_log.log(LOG_STATUS, "pp_update called\n");	
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
		
		bool success = dm->update(query->get("logs"));
		logChange();
		
		if (success) {
			_log.log(LOG_STATUS, "Successfully handled /pp/update query\n");
			return writeStatusAndEnd(req,res,200);
		} else {
			_log.log(LOG_WARN, "Failed to handle /pp/update query\n");
			return writeStatusAndEnd(req,res,500);
		}
	}
}
void PervertLayer::pp_list(Server::Request* req, Server::Response* res) {
	Json::Value root;
	Json::Value execs(Json::arrayValue);
	
	vector<Json::Value> execs_arr;
	map<string,DataManager*>::iterator it;
	for (it=_dms.begin(); it != _dms.end(); it++) {
		execs.append(Json::Value((*it).first.c_str()));
	}

	root["count"] = (int) _dms.size();
	root["execs"] = execs;	
	writeJSONResponse(req,res,root);
	
}
void PervertLayer::f_counts(Server::Request* req, Server::Response* res) {
  Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL || !query->exists("exec")) 
		return writeStatusAndEnd(req,res,500);
  
  string exec = query->get("exec");
  if ( _dms.find(exec) == _dms.end() )
    return writeStatusAndEnd(req,res,500);

  DataManager* dm = _dms[exec];
  Trace* trace = dm->getTrace();

  Json::Value root;

  root["event"]  = (int) trace->events_.size();
  root["malloc"] = (int) trace->byType_[Trace::Event::MALLOC].size();
  root["free"]   = (int) trace->byType_[Trace::Event::FREE].size();
  root["read"]   = (int) trace->byType_[Trace::Event::READ].size();
  root["write"]  = (int) trace->byType_[Trace::Event::WRITE].size();

  writeJSONResponse(req,res,root);
}
void PervertLayer::f_mem_status(Server::Request* req, Server::Response* res) {
  Server::QueryData* query = (Server::QueryData*) res->getMetadata("query");
	if (query == NULL || !query->exists("exec") || !query->exists("frame") || !query->exists("window")) 
		return writeStatusAndEnd(req,res,500);
  
  string exec = query->get("exec");
  if ( _dms.find(exec) == _dms.end() )
    return writeStatusAndEnd(req,res,500);

  DataManager* dm = _dms[exec];
  Trace* trace = dm->getTrace();

  int frame = atoi(query->get("frame").c_str());   
  int window = atoi(query->get("window").c_str());   

  if ( (unsigned int) frame >= trace->events_.size() )
    return writeStatusAndEnd(req,res,500);

  Json::Value root;
  Json::Value regions(Json::arrayValue);
  Json::Value events(Json::arrayValue);
  Json::Value addr(Json::arrayValue);
  Json::Value mask(Json::arrayValue);

  list<Trace::Event*>& validRegions = trace->validRegions_[frame];

  for ( list<Trace::Event*>::iterator i = validRegions.begin(), ie = validRegions.end(); i != ie; ++i )
  {
    Json::Value region;
    region["begin"] = (int) (*i)->arg1;
    region["end"]   = (int) (*i)->arg2;
    regions.append(region);
  }

  for ( unsigned int count = 0; frame >= 0 && count < (unsigned int) window; --frame )
  {
    const Trace::Event& event = trace->events_[frame];
    if ( event.type == Trace::Event::READ || event.type == Trace::Event::WRITE )
    {
      bool valid = false;
      for ( list<Trace::Event*>::iterator i = validRegions.begin(), ie = validRegions.end(); i != ie; ++i )
        if ( event.arg1 >= (*i)->arg1 && event.arg1 < (*i)->arg2 )
        {
          valid = true;
          break; 
        }
      
      events.append(Json::Value(event.type == Trace::Event::READ ? "r" : "w"));
      addr.append(Json::Value((int) event.arg1));
      mask.append(Json::Value(valid));

      count++;
    }
  }

  root["regions"] = regions;
  root["events"] = events;
  root["addr"] = addr;
  root["mask"] = mask;

  writeJSONResponse(req,res,root);
}
void PervertLayer::handle(Server::Request* req, Server::Response* res) {
	const struct mg_request_info *request_info = req->request_info;
	
	if (strcmp(request_info->uri, "/ping") == 0) {
		return ping(req,res);
	} else if (strcmp(request_info->uri, "/pp/update") == 0) {
		return pp_update(req,res);
	} else if (strcmp(request_info->uri, "/pp/list") == 0) {
		return pp_list(req,res);
	} else if (strcmp(request_info->uri, "/f/status") == 0) {
		return f_status(req,res);
  } else if (strcmp(request_info->uri, "/f/mem_status") == 0 ) {
    return f_mem_status(req,res);
  } else if (strcmp(request_info->uri, "/f/counts") == 0 ) {
    return f_counts(req,res);
	} else {
		next(req,res);
	}
}

const char* PervertLayer::name() {
	return "PervertLayer";
}

} /* namespace Server */
} /* namespace PerVERT */



