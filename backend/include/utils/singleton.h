//
//  singleton.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//
#ifndef _UTILS_SINGLETON_H_
#define _UTILS_SINGLETON_H_

#include <cstdio>

namespace Utils {

template <class T>
class Singleton {
public:
	static T& Instance() {
		static T _instance;
		return _instance;
	}
 	virtual ~Singleton() {
		printf("WOOOOOOOOOOOOOO\n");
	}

private:
	 Singleton();
	 Singleton(Singleton const&);
	 Singleton& operator=(Singleton const&);
};

} /* namespace Utils */

#endif /* _UTILS_SINGLETON_H_ */
