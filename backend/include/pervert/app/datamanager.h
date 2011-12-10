//
//  datamanager
//
//  Created by Niels Joubert on 2011-12-6.
//

#ifndef PERVERT_APP_DATAMANAGER_H_
#define PERVERT_APP_DATAMANAGER_H_

#include <cstring>
#include <pthread.h>

#include <iostream>
#include <fstream>

#include "utils/log.h"

#include "pervert/app/trace.h"

namespace PerVERT {
namespace App {
	
using namespace Utils;

class DataManager {
public:
	DataManager(string exec);
	~DataManager();
	string exec();
	
	bool update(string logs);
	bool status();

  Trace* getTrace() const { return _trace; } 

private:
	Log& _log;
	string _exec;
	pthread_mutex_t the_lock;
	
	void lock();
	void unlock();
	
	bool _busy;

  Trace* _trace;  
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_DATAMANAGER_H_ */ 
