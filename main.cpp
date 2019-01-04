#include <iostream>
#include "shared.h"
#include "cqueue/cqueue.h"
#include "adam6x50/adam6x50_thread.h"
#include "mbserver/mbserver_thread.h"
#include "SQLManager/sqlmanager_update_thread.h"
#include "datastruct/adamtag.h"

int main(int argc, char **argv)
{
	std::clog.rdbuf(new Log("homeplatform", LOG_LOCAL0));
	std::clog << kLogNotice << "*** starting homeplatform" << std::endl;
	
	cqueue<adamtag> cqReadToPLC;
	cqueue<adamtag> cqResponseFromPLC;
	cqueue<adamtag> cqWriteToPLC;

	cqueue<datatag> cqReadChange;
	cqueue<datatag> cqWriteRequest;	
	
	adam6x50_thread *adam6250 = new adam6x50_thread();
	mbserver_thread *mbserver = new mbserver_thread();
	sqlmanager_update_thread *sqlupdate = new sqlmanager_update_thread();

	mbserver->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	mbserver->SetIP("***REMOVED***");
	mbserver->SetQueueAdamReadToPLC(&cqReadToPLC);
	mbserver->SetQueueAdamResponseFromPLC(&cqResponseFromPLC);
	mbserver->SetQueueAdamWriteToPLC(&cqWriteToPLC);
	mbserver->SetQueueReadChange(&cqReadChange);
	mbserver->SetQueueWriteRequest(&cqWriteRequest);
	mbserver->ThreadStart();
	
	adam6250->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	adam6250->SetQueueAdamReadToPLC(&cqReadToPLC);
	adam6250->SetQueueAdamResponseFromPLC(&cqResponseFromPLC);
	adam6250->SetQueueAdamWriteToPLC(&cqWriteToPLC);
	adam6250->ThreadStart();
	
	sqlupdate->SetConnectionString("host=localhost ***REMOVED*** sslmode=disable user=postgres ***REMOVED***");
	sqlupdate->SetQueueReadChange(&cqReadChange);
	sqlupdate->ThreadStart();	
	
	
	sqlupdate->ThreadJoin();
	adam6250->ThreadJoin();
	mbserver->ThreadJoin();
	
	std::clog << kLogNotice << "*** ending homeplatform" << std::endl;

	return 0;
}
