//
//  pervertlayer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_APP_PERVERTLAYER_H_
#define PERVERT_APP_PERVERTLAYER_H_

#include <iostream>
#include <fstream>

#include "utils/server/layers/jsonlayer.h"
#include "utils/log.h"
#include "pervert/app/datamanager.h"


namespace PerVERT {
namespace App {
	
using namespace Utils;

class PervertLayer : public Server::Layers::JSONLayer {
public:
	PervertLayer();
	void handle(Server::Request* req, Server::Response* res);
	const char* name();
private:
	Log& _log;
	map<string,DataManager*> _dms;

	//call this whenever internal state changes.
	void logChange();	
	int _lastChangeID;

	void writePollResponse(Server::Request* req, Server::Response* res, const char* text);
	//HTTP methods we respond to:
	void ping(Server::Request* req, Server::Response* res);
	void pp_update(Server::Request* req, Server::Response* res);
	void pp_list(Server::Request* req, Server::Response* res);
	void f_status(Server::Request* req, Server::Response* res);
  void f_mem_status(Server::Request* req, Server::Response* res);	
  void f_counts(Server::Request* req, Server::Response* res);
  void f_context_stack(Server::Request* req, Server::Response* res);
  void f_context_events(Server::Request* req, Server::Response* res);
  void f_context_deriv(Server::Request* req, Server::Response* res);
  void f_context_histo(Server::Request* req, Server::Response* res);
};


} /* namespace App */
} /* namespace PerVERT */


#endif /* PERVERT_APP_PERVERTLAYER_H_ */ 
