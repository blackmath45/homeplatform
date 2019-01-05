#ifndef MBSERVER_THREAD_H
#define MBSERVER_THREAD_H

#include <pthread.h>
#include "mbserver_conf.h"
#include <modbus/modbus.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <errno.h>
#include "../cqueue/cqueue.h"
#include "../datastruct/adamtag.h"

class mbserver_thread
{
private :
	string sConnectionString;
	string sIP;
	cqueue<adamtag> *cqAdamReadToPLC;
	cqueue<adamtag> *cqAdamResponseFromPLC;
	cqueue<adamtag> *cqAdamWriteToPLC;
	cqueue<datatag> *cqReadChange;
	cqueue<datatag> *cqWriteRequest;	
	
	pthread_mutex_t *sqlmanager_update_thread_mutex;
	pthread_cond_t *sqlmanager_update_thread_condition;
	
	pthread_t thread_id;
	void threadFunction(void* args);
	
	struct threadArgsStruct
	{
		char *cConnectionString;
		char *cIP;
		cqueue<adamtag> *cqAdamReadToPLC;
		cqueue<adamtag> *cqAdamResponseFromPLC;
		cqueue<adamtag> *cqAdamWriteToPLC;
		cqueue<datatag> *cqReadChange;
		cqueue<datatag> *cqWriteRequest;
		pthread_mutex_t *sqlmanager_update_thread_mutex;
		pthread_cond_t *sqlmanager_update_thread_condition;
	} thargs;

public:
	void SetConnectionString(string sConnectionString);	
	void SetIP(string sIP);
	void SetQueueAdamReadToPLC(cqueue<adamtag> *cqAdamReadToPLC);
	void SetQueueAdamResponseFromPLC(cqueue<adamtag> *cqAdamResponseFromPLC);
	void SetQueueAdamWriteToPLC(cqueue<adamtag> *cqAdamWriteToPLC);
	void SetQueueReadChange(cqueue<datatag> *cqReadChange);
	void SetQueueWriteRequest(cqueue<datatag> *cqWriteRequest);
	void SetThreadMutex(pthread_mutex_t *sqlmanager_update_thread_mutex);
	void SetThreadCondition(pthread_cond_t *sqlmanager_update_thread_condition);
	static void *ThreadWrapper(void *thisPtr)
	{
		((mbserver_thread*) thisPtr)->threadFunction(thisPtr);
		return NULL;
	}
	
	void ThreadStart();
	void ThreadJoin();
};

//TODO function STOP + JOIN + Passage pointeur queue

#endif // MBSERVER_THREAD_H
