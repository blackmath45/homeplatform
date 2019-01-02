#ifndef MBSERVER_CONF_H
#define MBSERVER_CONF_H

#include <postgresql/libpq-fe.h>
#include "../shared.h"
#include <stdlib.h> 
#include <list>

using namespace std;

class mbserver_conf
{
private:
	PGconn *conn;
	string sConnectionString;
	string sQuery;

	list<datatag> ldatatag;
	int datatagcount;
	
public:
	mbserver_conf();
	void SetConnectionString(string sConnectionString);
	int LoadConf();
	
	list<datatag> *GetList();
};

//TODO Faire fonction free pour la liste ?

#endif // MBSERVER_CONF_H