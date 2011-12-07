#include "pervert/app/datamanager.h"

namespace PerVERT {
namespace App {

DataManager::DataManager(string exec)  : _log(GETLOG("DATAMANAGER")){
	_log.log(LOG_STATUS, "datamamanger created...\n");
	_exec = exec;
}

//Returns 0 on success
int DataManager::update(string logs) {
	return 0; //success
}



} /* namespace Server */
} /* namespace PerVERT */



