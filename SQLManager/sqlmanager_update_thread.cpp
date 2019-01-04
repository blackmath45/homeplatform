#include "sqlmanager_update_thread.h"

void sqlmanager_update_thread::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }
void sqlmanager_update_thread::SetQueueReadChange(cqueue<datatag> *cqReadChange) { this->cqReadChange = cqReadChange; }
	
void sqlmanager_update_thread::ThreadStart()
{
	thargs.cConnectionString = strdup(this->sConnectionString.c_str());

	thargs.cqReadChange = this->cqReadChange;	

	std::clog << kLogNotice << "sqlmanager_update_thread : starting thread" << std::endl;
	pthread_create(&this->thread_id, NULL, &sqlmanager_update_thread::ThreadWrapper, (void *)&thargs);
}

void sqlmanager_update_thread::ThreadJoin()
{
	pthread_join(this->thread_id, NULL);
	std::clog << kLogNotice << "sqlmanager_update_thread : thread join" << std::endl;
}

void sqlmanager_update_thread::threadFunction(void* args)
{
	std::clog << kLogNotice << "sqlmanager_update_thread : thread started" << std::endl;
	
	//*************** ARGS ***************
	struct threadArgsStruct *thargs = (struct threadArgsStruct*)args;
	char *cConnectionString;
	cConnectionString = (char *)thargs->cConnectionString;

	string sConnectionString(cConnectionString);	

	cqueue<datatag> *cqReadChange = (cqueue<datatag> *)thargs->cqReadChange;
	//************************************
	
	PGresult   *result;
	
	this->conn = PQconnectdb(this->sConnectionString.c_str());
	
	if (PQstatus(this->conn) != CONNECTION_OK)
	{
		std::clog << kLogWarning << "sqlmanager_update_thread : Connection to database failed | " << PQerrorMessage(conn) << std::endl;
		PQfinish(this->conn);
	}
	else
	{
		while(cqReadChange->size() > 0)
		{
			datatag tmp;
			cqReadChange->pop(tmp);
			
			printf("---------- Save Value ----------\n");
			
			ostringstream osID;
			ostringstream osMBAddress;
			ostringstream osMBType;
			ostringstream osQuality;
			
			osID << tmp.GetID();
			osMBAddress << tmp.GetMBAddress();
			osMBType << tmp.GetMBType();
			osQuality << tmp.GetQuality();
			
			this->sQuery = "SELECT \"SP_MB_Tags_Update\"(" + osID.str() + "," + osMBAddress.str() + "," + osMBType.str() + ",'" + tmp.GetValue() + "'," + osQuality.str() + ");";
			result = PQexec(conn, sQuery.c_str());
			if (PQresultStatus(result) != PGRES_COMMAND_OK)
			{
				std::clog << kLogWarning << "sqlmanager_update_thread : Query failed | " << PQerrorMessage(conn) << std::endl;
				PQclear(result);
			}
			else
			{
				PQclear(result);
			}		
		}
	}
	
	PQfinish(this->conn);	

	std::clog << kLogNotice << "sqlmanager_update_thread : end of thread" << std::endl;
	
	return;
}