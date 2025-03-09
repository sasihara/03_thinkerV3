#pragma once
#include "stdio.h"
#include "stdarg.h"
#include "limits.h"

enum LogLevel {
	LOGLEVEL_CRITICAL = 0,
	LOGLEVEL_ERROR,
	LOGLEVEL_WARNING,
	LOGLEVEL_INFO,
	LOGLEVEL_DEBUG,
	LOGLEVEL_TRACE,
	LOGLEVEL_ALL
};

class Logging {
public:
	int init(int level, const char* logPath);
	int logprintf(int _level, const char* format, ...);
	int logprintf(const char* format, ...);
	int logout(int _level, const char *format, ...);
	int logout(const char* format, ...);
	int flush();
	int end();
private:
	bool initialized = false;
	FILE* f;
	int level = INT_MAX;

	int logprintf(int _level, const char* _format, va_list _args);
	int logout(int _level, const char* format, va_list _args);
};