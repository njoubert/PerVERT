//
//  config.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//
#ifndef PERVERT_SERVER_CONFIG_H_
#define PERVERT_SERVER_CONFIG_H_


namespace PerVERT {
namespace Server {
		
struct Config {
	bool makeDaemon;
	Config() {
		makeDaemon = false;
	}
};

} /* namespace Server */
} /* namespace PerVERT */

#endif  // PERVERT_SERVER_CONFIG_H_
