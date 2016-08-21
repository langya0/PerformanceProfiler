#pragma once 
#include <strstream>
using namespace std;

#include <pthread.h>
#include <time.h>
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>

#include "trace.h"
#include "singleton.h"
#include "output.h"
#include "configManager.h"

typedef long long longType;
static int nox = 1;
//剖析部件3
class ppNode
{
protected:
	string _file;	//文件名
	longType _line;	//行号
	string _func;	//函数名
	string _desc;	//描述
public:
	longType _nox;	//序号

	ppNode(string file, longType line,string func,const char* desc)
	:_file(file)
	,_line(line)
	,_func(func)
	,_desc(desc)
	,_nox(nox)
	{
		Trace("");
	}
public:
	void output(outputAdapt & out)const 
	{
			out.output("NO.%lld, Description:%s\nFile:%s, Line:%lld, Function:%s\n"
				,_nox
				,_desc.c_str()
				,_file.c_str()
				,_line
				,_func.c_str());
	}
public:
	bool operator<(const ppNode& p)const 
	{
		if (_line > p._line)
		return false;

		if (_line < p._line)
			return true;

		if (_file > p._file)
			return false;
		
		if (_file < p._file)
			return true;

		if (_func > p._func)
			return false;

		if (_func < p._func)
			return true;

		return false;
	}
};

class ppSection
{
protected:
	map<int,longType> _mapBeginTime;
	map<int,longType> _mapCallCount;
	map<int,longType> _mapCostTime;
	map<int,longType> _mapRef;

	pthread_mutex_t lock;
	friend class performanceProfiler;
	longType _totalCostTime;
	longType _totalCallCount;
	longType _totalRef;
public:
	ppSection()
	:_totalRef(0)
	,_totalCallCount(0)
	,_totalCostTime(0)
	{
		pthread_mutex_init(&lock,NULL);
	}
	void begin(int threadId)
	{
		pthread_mutex_lock(&lock);

		if (++_mapRef[threadId]==1)
		{
			_mapBeginTime[threadId] = clock();//首次进入获取开始时间
		}
		++_mapCallCount[threadId];	//调用次数增加
		_totalCallCount+=1;//总调用次数增加
		++_totalRef;

		pthread_mutex_unlock(&lock);
	}

	void end(int threadId)
	{
		pthread_mutex_lock(&lock);

		if(--_mapRef[threadId]==0)//计数减少
		{
			_mapCostTime[threadId] = clock() - _mapBeginTime[threadId];//线程花费时间

			_totalCostTime+=_mapCostTime[threadId];	//所有线程时间累加
		}
		;
		--_totalRef;
		pthread_mutex_unlock(&lock);
	}

	void output(outputAdapt & out)
	{
		map<int,longType>::iterator it = _mapCostTime.begin();
		while(it != _mapCostTime.end())
		{
			out.output("ThreadId:%d, CostTime:%lld, CallCount:%lld\n",it->first\
				,it->second
				,_mapCallCount[it->first]);
			++it;
		}
	}
};
class performanceProfiler:public singleton<performanceProfiler>
{
	friend class singleton<performanceProfiler>;
	pthread_mutex_t lock;
protected:
	performanceProfiler()
	{
		pthread_mutex_init(&lock,NULL);
		Trace("");
	}

	performanceProfiler(const performanceProfiler&);
	performanceProfiler& operator=(const performanceProfiler&);
public:
	ppSection* createSection(const char* file, const char* func,const int line, const char* desc)
	{
		pthread_mutex_lock(&lock);
		ppNode node(file,line,func,desc);
		Trace(" ssmap->size = %d",ssmap.size());
		if(ssmap[node] == NULL)
		{
			Trace("new ppSection");
			ssmap[node] = new ppSection();
			++nox;
		}
		Trace(" ssmap->size = %d",ssmap.size());
		pthread_mutex_unlock(&lock);
		return ssmap[node];
	}

	void output(outputAdapt & out)
	{
		_output(out);
	}

	~performanceProfiler()
	{
		Trace("");
	}
protected:
	typedef std::map<ppNode,ppSection*>::iterator mapit;

	struct cmp
	{
	public:
		bool operator()(const mapit&l,const mapit&r)
		{
			return true;
		}
	};
	void _output(outputAdapt& out)
	{
		out.output("=========performance Profiler Report=========\n");
		time_t timep;  
	    char s[30];  
	    strcpy(s,ctime(&timep));  
		out.output("Profiler Begin time:%s\n",s);//打印剖析器启动时间


		mapit its = ssmap.begin();
		vector<mapit> v;
		for(;its!= ssmap.end();++its)
		{
			v.push_back(its);
		}
		// std::vector<mapit> v(ssmap.begin(),ssmap.end());

		sort(v.begin(),v.end(),cmp());

		std::vector<mapit>::iterator it = v.begin();
		while(it != v.end())
		{
			if((*it)->second->_totalRef!=0)
				out.output("ref errno!");
			(*it)->first.output(out);
			(*it)->second->output(out);
			out.output("Total CostTime:%lld\t",(*it)->second->_totalCostTime);
			out.output("Total CallCount:%lld\n\n",(*it)->second->_totalCallCount);
			++it;
		}
		out.output("=============================================\n");
	}
	map<ppNode,ppSection*> ssmap;
};

// 程序执行完成输出
struct RAII
{
	~RAII()
	{
		if(configManager::getInstace()->getOptions()&SAVEFILE)
		{
			outputLog ol;
			singleton<performanceProfiler>::getInstace()->output(ol);	
		}

		if(configManager::getInstace()->getOptions()&SAVECONSOLE)
		{
			outputTerm ot;
			singleton<performanceProfiler>::getInstace()->output(ot);	
		}
	}
};
RAII raii;

//如果配置使用性能分析,那么执行,否则空
#define PERFORMANCEPROFILER_BEGIN(sign,desc)			\
	ppSection* sign##sec = NULL;						\
	if(configManager::getInstace()->getOptions()&ENABLE)\
	{													\
	 	sign##sec = singleton<performanceProfiler>::getInstace()->createSection(__FILE__,__func__,__LINE__,desc);\
		sign##sec->begin(pthread_self());				\
	}

#define PERFORMANCEPROFILER_END(sign)\
	if(configManager::getInstace()->getOptions()&ENABLE)	\
	{													\
		sign##sec->end(pthread_self());							\
	}