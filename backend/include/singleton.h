//
//  singleton.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//
#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <cstdio>

namespace Backend {

template <class T>
class Singleton {
public:
	static T& Instance() {
		static T _instance;
		return _instance;
	}
 	~Singleton() {
		printf("WOOOOOOOOOOOOOO\n");
	}

private:
	 Singleton();
	 Singleton(Singleton const&);
	 Singleton& operator=(Singleton const&);
};

} /* namespace Backend */

#endif /* _SINGLETON_H_ */
