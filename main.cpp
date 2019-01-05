#include <iostream>
#include "shared.h"
#include "cqueue/cqueue.h"
#include "adam6x50/adam6x50_thread.h"
#include "mbserver/mbserver_thread.h"
#include "SQLManager/sqlmanager_update_thread.h"
#include "SQLManager/sqlmanager_writes_thread.h"
#include "datastruct/adamtag.h"

int main(int argc, char **argv)
{
	std::clog.rdbuf(new Log("homeplatform", LOG_LOCAL0));
	std::clog << kLogNotice << "*** starting homeplatform" << std::endl;
	
	pthread_mutex_t sqlmanager_update_thread_mutex;
	pthread_cond_t sqlmanager_update_thread_condition;
	
	cqueue<adamtag> cqReadToPLC;
	cqueue<adamtag> cqResponseFromPLC;
	cqueue<adamtag> cqWriteToPLC;

	cqueue<datatag> cqReadChange;
	cqueue<datatag> cqWriteRequest;	
	
	pthread_mutex_init(&sqlmanager_update_thread_mutex, NULL);
	pthread_cond_init (&sqlmanager_update_thread_condition, NULL);

	adam6x50_thread *adam6250 = new adam6x50_thread();
	mbserver_thread *mbserver = new mbserver_thread();
	sqlmanager_update_thread *sqlupdate = new sqlmanager_update_thread();
	sqlmanager_writes_thread *sqlwrite = new sqlmanager_writes_thread();

	mbserver->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	mbserver->SetIP("***REMOVED***");
	mbserver->SetQueueAdamReadToPLC(&cqReadToPLC);
	mbserver->SetQueueAdamResponseFromPLC(&cqResponseFromPLC);
	mbserver->SetQueueAdamWriteToPLC(&cqWriteToPLC);
	mbserver->SetQueueReadChange(&cqReadChange);
	mbserver->SetQueueWriteRequest(&cqWriteRequest);
	mbserver->SetThreadMutex(&sqlmanager_update_thread_mutex);
	mbserver->SetThreadCondition(&sqlmanager_update_thread_condition);
	mbserver->ThreadStart();
	
	adam6250->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	adam6250->SetQueueAdamReadToPLC(&cqReadToPLC);
	adam6250->SetQueueAdamResponseFromPLC(&cqResponseFromPLC);
	adam6250->SetQueueAdamWriteToPLC(&cqWriteToPLC);
	adam6250->ThreadStart();
	
	//TODO : https://gist.github.com/itrobotics/4bdc417071f560b220ba
	//TODO : pour éviter les sleep
	sqlupdate->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	sqlupdate->SetQueueReadChange(&cqReadChange);
	sqlupdate->SetThreadMutex(&sqlmanager_update_thread_mutex);
	sqlupdate->SetThreadCondition(&sqlmanager_update_thread_condition);
	sqlupdate->ThreadStart();	

	sqlwrite->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	sqlwrite->SetQueueWriteRequest(&cqWriteRequest);
	sqlwrite->ThreadStart();	
	
	
	sqlwrite->ThreadJoin();	
	sqlupdate->ThreadJoin();
	adam6250->ThreadJoin();
	mbserver->ThreadJoin();

	pthread_mutex_destroy(&sqlmanager_update_thread_mutex);
	pthread_cond_destroy(&sqlmanager_update_thread_condition);
   
	std::clog << kLogNotice << "*** ending homeplatform" << std::endl;

	return 0;
}
