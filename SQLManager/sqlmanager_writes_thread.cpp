#include "sqlmanager_writes_thread.h"

void sqlmanager_writes_thread::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }
void sqlmanager_writes_thread::SetQueueWriteRequest(cqueue<datatag> *cqWriteRequest) { this->cqWriteRequest = cqWriteRequest; }
	
void sqlmanager_writes_thread::ThreadStart()
{
	thargs.cConnectionString = strdup(this->sConnectionString.c_str());

	thargs.cqWriteRequest = this->cqWriteRequest;	

	std::clog << kLogNotice << "sqlmanager_writes_thread : starting thread" << std::endl;
	pthread_create(&this->thread_id, NULL, &sqlmanager_writes_thread::ThreadWrapper, (void *)&thargs);
}

void sqlmanager_writes_thread::ThreadJoin()
{
	pthread_join(this->thread_id, NULL);
	std::clog << kLogNotice << "sqlmanager_writes_thread : thread join" << std::endl;
}

void sqlmanager_writes_thread::threadFunction(void* args)
{
	std::clog << kLogNotice << "sqlmanager_writes_thread : thread started" << std::endl;
	
	//*************** ARGS ***************
	struct threadArgsStruct *thargs = (struct threadArgsStruct*)args;
	char *cConnectionString;
	cConnectionString = (char *)thargs->cConnectionString;

	string sConnectionString(cConnectionString);	

	cqueue<datatag> *cqWriteRequest = (cqueue<datatag> *)thargs->cqWriteRequest;
	//************************************
	
	PGresult   *result;
	string sQuery;
	list<datatag> ldatatag;
	int datatagcount;
	int i;
	
	this->conn = PQconnectdb(this->sConnectionString.c_str());
	
	if (PQstatus(this->conn) != CONNECTION_OK)
	{
		std::clog << kLogWarning << "sqlmanager_writes_thread : Connection to database failed | " << PQerrorMessage(conn) << std::endl;
		PQfinish(this->conn);
	}
	else
	{
		while(1)
		{
			//*****************************************************************************************
			// Récupération
			//*****************************************************************************************
			sQuery = "SELECT \"SP_MB_Tags_WriteOrders_Get\"();";
			result = PQexec(conn, sQuery.c_str());
			if (PQresultStatus(result) != PGRES_TUPLES_OK)
			{
				std::clog << kLogWarning << "sqlmanager_writes_thread : Query failed | " << PQerrorMessage(conn) << std::endl;
			}
			else
			{
				datatagcount = PQntuples(result);

				for (i = 0; i < datatagcount; i++)
				{							
					datatag tmp;

					tmp.SetFromDBID(PQgetvalue(result, i, 0));
					tmp.SetFromDBMBAddress(PQgetvalue(result, i, 1));
					tmp.SetFromDBMBType(PQgetvalue(result, i, 2));
					tmp.SetFromDBValue(PQgetvalue(result, i, 3));
					tmp.SetFromDBQuality(0);
					
					ldatatag.insert(ldatatag.end(),tmp);
				}
			}

			PQclear(result);
			
			//*****************************************************************************************
			// Transfert et Acquittement
			//*****************************************************************************************
			for(list<datatag>::iterator ldatatagIter = ldatatag.begin(); ldatatagIter != ldatatag.end(); ldatatagIter ++)
			{
				cqWriteRequest->push(*ldatatagIter);
				
				ostringstream osID;

				osID << ldatatagIter->GetID();
				
				sQuery = "SELECT \"SP_MB_Tags_WriteOrders_Ack\"(" + osID.str() + ");";
				result = PQexec(conn, sQuery.c_str());
				if (PQresultStatus(result) != PGRES_TUPLES_OK)
				{
					std::clog << kLogWarning << "sqlmanager_writes_thread : Query ack failed | " << PQerrorMessage(conn) << std::endl;
					PQclear(result);
				}
				else
				{
					PQclear(result);
				}	
			}				
			
			sleep(1);
		}
	}
	
	PQfinish(this->conn);	

	std::clog << kLogNotice << "sqlmanager_writes_thread : end of thread" << std::endl;
	
	return;
}