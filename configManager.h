#pragma once
#include "trace.h"
#include "singleton.h"
enum OPTIONS
{
	DISABLE=0,
	ENABLE=1,
	SAVEFILE=2,
	SAVECONSOLE=4,
	ORDERTOTALTIME=8,
	ORDERCALLCOUNT=16,
};
struct configManager:public singleton<configManager>
{
	friend class singleton<configManager>;
	int getOptions()
	{
		return _options;
	}
	void setOptions(int options)
	{
		_options |=options;
		// Trace("options=%d",_options);
	}
	void delOptions(int options)
	{
		_options &=(~options);
	}
private:
	int _options;
};