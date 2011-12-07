//
//  pervertlayer
//
//  Created by Niels Joubert on 2011-11-30.
//

#ifndef PERVERT_APP_PERVERTLAYER_H_
#define PERVERT_APP_PERVERTLAYER_H_

#include <iostream>
#include <fstream>

#include "pervert/server/httplayer.h"
#include "utils/log.h"
#include "pervert/app/datamanager.h"


namespace PerVERT {
namespace App {
	
using namespace Utils;

class PervertLayer : public Server::HTTPLayer {
public:
	PervertLayer();
	void handle(Server::Request* req, Server::Response* res);
	const char* name();
private:
	Log& _log;
	
	DataManager _dataManager;
	
	//HTTP methods:
	
	void ping(Server::Request* req, Server::Response* res);
	void pp_update(Server::Request* req, Server::Response* res);

	void f_status(Server::Request* req, Server::Response* res);
	

};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_PERVERTLAYER_H_ */ 