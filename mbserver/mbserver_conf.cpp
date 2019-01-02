#include "mbserver_conf.h"

mbserver_conf::mbserver_conf()
{
	this->sQuery = "SELECT \"ID\", \"ModBus_Address\", \"ModBus_Type\", \"Valeur\", \"Quality\" FROM \"SP_MB_Tags_GetList\"();";
}
	
void mbserver_conf::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }

int mbserver_conf::LoadConf()
{	
	PGresult   *result;
	this->datatagcount = 0;
	int i;
	
	this->ldatatag.clear();
	
	this->conn = PQconnectdb(this->sConnectionString.c_str());
	
	if (PQstatus(this->conn) != CONNECTION_OK)
	{
		std::clog << kLogWarning << "mbserver_conf : Connection to database failed | " << PQerrorMessage(conn) << std::endl;
		PQfinish(this->conn);
		return -1;
	}

	result = PQexec(conn, sQuery.c_str());
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		std::clog << kLogWarning << "mbserver_conf : Query failed | " << PQerrorMessage(conn) << std::endl;
	}
	else
	{
		this->datatagcount = PQntuples(result);

		for (i = 0; i < this->datatagcount; i++)
		{							
			datatag tmp;

			tmp.SetFromDBID(PQgetvalue(result, i, 0));
			tmp.SetFromDBMBAddress(PQgetvalue(result, i, 1));
			tmp.SetFromDBMBType(PQgetvalue(result, i, 2));
			tmp.SetFromDBValue(PQgetvalue(result, i, 3));
			tmp.SetFromDBQuality(PQgetvalue(result, i, 4));
			
			this->ldatatag.insert(ldatatag.end(),tmp);
		}
		
		if (this->datatagcount == 0)
			std::clog << kLogWarning << "mbserver_conf : Pas d'enregistrement !" << std::endl;
	}

	PQclear(result);	

	PQfinish(this->conn);	
	
	return 0;
}

list<datatag> *mbserver_conf::GetList() { return &ldatatag; };