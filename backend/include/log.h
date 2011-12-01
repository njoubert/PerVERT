//
//
//  server.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <string>
#include "singleton.h"

#define GETLOG(className) Singleton<LogFactory>::Instance().getLog(className)

namespace Backend {

using namespace std;

enum LOGLEVEL {
	LOG_OFF     = 0,     //don't log ANYTHING
	LOG_ERROR   = 1,   //something fucked up
	LOG_MESSAGE = 2, //message to the user, not inside loop
	LOG_WARN    = 3,
	LOG_STATUS  = 4,  //status of system, periodic but not often
	LOG_INFO    = 5,    //might be inside loops
	LOG_DEBUG   = 6    //dumping like your grandma
};

const char* levelToChar(LOGLEVEL level);

class Log {
public:
	Log();
	Log(string className, LOGLEVEL level);
	~Log();
	void setLevel(LOGLEVEL level);
	LOGLEVEL getLevel();
	void log(LOGLEVEL level, const char* msg, ...);
private:
	string _className;
	LOGLEVEL _level;
};

class LogFactory {
public:
	LogFactory();
	~LogFactory();
	Log& getLog(string className);
	void setDefaultLevel(LOGLEVEL level);
	void setAllLogLevel(LOGLEVEL level);

private:
	LOGLEVEL _level;
	map<string, Log> logs;
};

} /* namespace Backend */


#endif  // LOG_H_