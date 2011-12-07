//
//  datamanager
//
//  Created by Niels Joubert on 2011-12-6.
//

#ifndef PERVERT_APP_DATAMANAGER_H_
#define PERVERT_APP_DATAMANAGER_H_

#include <iostream>
#include <fstream>

#include "utils/log.h"
#include <cstring>

namespace PerVERT {
namespace App {
	
using namespace Utils;

class DataManager {
public:
	DataManager(string exec);

private:
	Log& _log;
	string _exec;
	
};


} /* namespace Server */
} /* namespace PerVERT */


#endif /* PERVERT_APP_DATAMANAGER_H_ */ 