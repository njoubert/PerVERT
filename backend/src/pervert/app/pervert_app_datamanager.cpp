#include "pervert/app/datamanager.h"

namespace PerVERT {
namespace App {

DataManager::DataManager(string exec)  : _log(GETLOG("DATAMANAGER")){
	_log.log(LOG_STATUS, "datamamanger created...\n");
	_exec = exec;
	pthread_mutex_init(&the_lock,0);
}
DataManager::~DataManager() {
	pthread_mutex_destroy(&the_lock);
}
string DataManager::exec() {
	return _exec;
}

//Returns 0 on success
int DataManager::update(string logs) {
	lock();
	sleep(4);
	unlock();
	return 0; //success
}
bool DataManager::status() {
	if (pthread_mutex_trylock(&the_lock) != 0) {
		return true;
	} else {
		pthread_mutex_unlock(&the_lock);
		return false;
	}
	
}

void DataManager::lock() {
	pthread_mutex_lock(&the_lock);

}
void DataManager::unlock() {
	pthread_mutex_unlock(&the_lock);

}




} /* namespace Server */
} /* namespace PerVERT */



