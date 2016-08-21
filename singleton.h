#pragma once
#include <iostream>
using namespace std;
#include "trace.h"

template<class T>
class singleton
{
protected:
	static T* _instance;
	singleton()
	{
		Trace("");
	}
	singleton(const singleton&);
	singleton& operator=(const singleton&);
public:
	static T* getInstace();
	~singleton()
	{
		Trace("");
	}
};

template<class T>
T* singleton<T>::_instance = new T();
template<class T>
T* singleton<T>::getInstace()
{
	return _instance;
}
