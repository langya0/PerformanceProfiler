#pragma once
#include <stdio.h>
#include <stdarg.h>
#include "trace.h"

class outputAdapt
{
public:
	virtual void output(const char* format,...)=0;
};

class outputTerm:public outputAdapt
{
public:
	outputTerm(){}
	void output(const char* format,...)
	{
		va_list vl;
		va_start(vl,format);
		vfprintf(stdout,format,vl);
		va_end(vl);
	}
private:
	outputTerm(const outputTerm&);
	outputTerm& operator=(const outputTerm&);
};
class outputLog:public outputAdapt
{
protected:
	FILE* _fd;
public:
	outputLog()
	:_fd(NULL)
	{
		_fd = fopen("./log","w");
		if(_fd == NULL)
		{
			perror("fopen");
			throw;
		}
	}
	void output(const char * format,...)
	{
		va_list vl;
		va_start(vl,format);
		vfprintf(_fd,format,vl);
		va_end(vl);
	}
	~outputLog()
	{
		if (_fd != NULL)
		{
			fclose(_fd);
			_fd = NULL;
		}		
	}
private:
	outputLog(const outputLog&);
	outputLog& operator=(const outputLog&);
};