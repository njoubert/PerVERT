#include "utils/log.h"

namespace Utils {

#define LOGCR " \n	"

const char* levelToChar(LOGLEVEL level) {
	switch(level) {
		case LOG_OFF:
		return "OFF ";
		case LOG_ERROR:
		return "ERR ";
		case LOG_MESSAGE:
		return "MSG ";
		case LOG_WARN:
		return "WARN";
		case LOG_STATUS:
		return "STAT";
		case LOG_INFO:
		return "INFO";
		case LOG_DEBUG:
		return "DBG ";
	}
	return "OOB";
}

LogFactory::LogFactory() {
	_level = LOG_INFO;
	_fp = stderr;
}
LogFactory::~LogFactory() {
	
}
Log& LogFactory::getLog(string className) {
	if (logs.count(className) > 0) 
		return logs[className];
	if(20 > className.size())
        className.insert(className.size(), 20 - className.size(), ' ');
	Log l(className, _level, _fp);
	logs[className] = l;
	return logs[className];
}

void LogFactory::setAllLogLevel(LOGLEVEL level) {
	setDefaultLevel(level);
	map<string, Log>::iterator it;

	for ( it=logs.begin() ; it != logs.end(); it++ ) {
		(*it).second.setLevel(level);
	}

}

void LogFactory::setAllLogTo(FILE* fp) {
	_fp = fp;
	map<string, Log>::iterator it;

	for ( it=logs.begin() ; it != logs.end(); it++ ) {
		(*it).second.log_to(fp);
	}

}


void LogFactory::setDefaultLevel(LOGLEVEL level) {
	if (level > LOG_DEBUG)
		level = LOG_DEBUG;
	if (level < LOG_OFF)
		level = LOG_OFF;
	_level = level;
}	

Log::Log() {
	_level = LOG_DEBUG;
	_className = "";
}
Log::Log(string className, LOGLEVEL level, FILE* fp) {
	_fp = fp;
	_level = level;
	_className = className;
}

Log::~Log() {
	
}

void Log::log_to(FILE* fp) {
	_fp = fp;
}

void Log::setLevel(LOGLEVEL level) {
	if (level > LOG_DEBUG)
		level = LOG_DEBUG;
	if (level < LOG_OFF)
		level = LOG_OFF;
	_level = level;
}

LOGLEVEL Log::getLevel() {
	return _level;
}

void Log::log_cr(LOGLEVEL level) {
	if (level > _level)
		return;
	fprintf(_fp, LOGCR);
	fflush(_fp);
	
}
	
//uses printf-style formatting
void Log::log(LOGLEVEL level, const char* msg, ...) {
	if (level > _level)
		return;
	
	va_list argp;
	va_start(argp, msg);
	fprintf(_fp, "%s: %s ", levelToChar(level), _className.c_str());
	fflush(stdout);
	vfprintf(_fp, msg, argp);
	va_end(argp);	
}

} /* namespace Utils */
