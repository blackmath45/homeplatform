#include "adam6x50_conf.h"

adam6x50_conf::adam6x50_conf()
{
	this->sQuery = "SELECT \"PLC_ModBus_Address_Com\", \"PLC_ModBus_Address_Input\", \"PLC_ModBus_Address_Output\" FROM \"MB_Tags_ADAM\";";
	this->PLCMBAddress_Com = -1;
	this->PLCMBAddress_Input = -1;
	this->PLCMBAddress_Output = -1;
}
	
void adam6x50_conf::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }

int adam6x50_conf::LoadConf()
{	
	PGresult   *result;
	
	this->PLCMBAddress_Com = -1;
	this->PLCMBAddress_Input = -1;
	this->PLCMBAddress_Output = -1;	
	
	this->conn = PQconnectdb(this->sConnectionString.c_str());
	
	if (PQstatus(this->conn) != CONNECTION_OK)
	{
		std::clog << kLogWarning << "adam6x50_conf : Connection to database failed | " << PQerrorMessage(conn) << std::endl;
		PQfinish(this->conn);
		return -1;
	}

	result = PQexec(conn, sQuery.c_str());
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		std::clog << kLogWarning << "adam6x50_conf : Query failed | " << PQerrorMessage(conn) << std::endl;
	}
	else
	{
		if (PQntuples(result) == 1)
		{
			this->PLCMBAddress_Com = atoi(PQgetvalue(result, 0, 0));
			this->PLCMBAddress_Input = atoi(PQgetvalue(result, 0, 1));
			this->PLCMBAddress_Output = atoi(PQgetvalue(result, 0, 2));
		}
		else
		{
			std::clog << kLogWarning << "adam6x50_conf : Plus d'un enregistrement !" << std::endl;
		}
	}

	PQclear(result);	

	PQfinish(this->conn);	
	
	return 0;
}
	
int adam6x50_conf::GetPLCMBAddress_Com() { return this->PLCMBAddress_Com; };
int adam6x50_conf::GetPLCMBAddress_Input() { return this->PLCMBAddress_Input; };
int adam6x50_conf::GetPLCMBAddress_Output() { return this->PLCMBAddress_Output; };