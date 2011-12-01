//
//  config.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//
#ifndef SERVER_CONFIG_H_
#define SERVER_CONFIG_H_


namespace Backend {
namespace Server {
		
struct Config {
	bool makeDaemon;
	Config() {
		makeDaemon = false;
	}
};

} /* namespace Server */
} /* namespace Backend */

#endif  // SERVER_CONFIG_H_
