#include "pervert/app/datamanager.h"

namespace PerVERT {
namespace App {

DataManager::DataManager(string exec)  : _log(GETLOG("DATAMANAGER")){
	_log.log(LOG_STATUS, "datamamanger created...\n");
	_exec = exec;
	
}



} /* namespace Server */
} /* namespace PerVERT */



