//
//  config.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//
#ifndef _UTILS_SERVER_CONFIG_H_
#define _UTILS_SERVER_CONFIG_H_


namespace Utils {
namespace Server {
		
struct Config {
	bool makeDaemon;
	Config() {
		makeDaemon = false;
	}
};

} /* namespace Server */
} /* namespace PerVERT */

#endif  // _UTILS_SERVER_CONFIG_H_
