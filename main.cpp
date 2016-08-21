#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;
// #define __DEBUG
#include "trace.h"
#include "performanceProfiler.h"
void *test(void*)
{
	PERFORMANCEPROFILER_BEGIN(test,"test");
	sleep(1);
	Trace("");
	PERFORMANCEPROFILER_END(test);
}
void *fuck(void*)
{
	PERFORMANCEPROFILER_BEGIN(fuck,"fuck");
	Trace("");
	PERFORMANCEPROFILER_END(fuck);
}
int Fib(int n)
{
	PERFORMANCEPROFILER_BEGIN(Fib,"fib");
	int ret = 0;
	if (n<2)
		ret = n;
	else
		ret = Fib(n-1)+Fib(n-2);
	PERFORMANCEPROFILER_END(Fib);
	return ret;
}
int main(int argc, char const *argv[])
{
	configManager::getInstace()->setOptions(ENABLE);
	configManager::getInstace()->setOptions(SAVECONSOLE);
	configManager::getInstace()->setOptions(SAVEFILE);


	PERFORMANCEPROFILER_BEGIN(fib,"fib");
	Fib(10);
	PERFORMANCEPROFILER_END(fib);

  
    // printf("%s\n", s); 
    pthread_t th1,th2,th3,th4;
    pthread_create(&th1,NULL,test,NULL);
    pthread_create(&th2,NULL,test,NULL);

    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    pthread_create(&th3,NULL,fuck,NULL);
    pthread_create(&th4,NULL,fuck,NULL);
    pthread_join(th3,NULL);
    pthread_join(th4,NULL);
	test(NULL);
	// fuck(NULL);
	// fuck(NULL);
	// test(NULL);
	return 0;
}
