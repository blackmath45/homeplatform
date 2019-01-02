#ifndef ADAM6X50_CONF_H
#define ADAM6X50_CONF_H

#include <postgresql/libpq-fe.h>
#include "../shared.h"
#include <stdlib.h> 

using namespace std;

class adam6x50_conf
{
private:
	PGconn *conn;
	string sConnectionString;
	string sQuery;
	int PLCMBAddress_Com;
	int PLCMBAddress_Input;
	int PLCMBAddress_Output;
	
public:
	adam6x50_conf();
	void SetConnectionString(string sConnectionString);
	int LoadConf();
	int GetPLCMBAddress_Com();
	int GetPLCMBAddress_Input();
	int GetPLCMBAddress_Output();
};

#endif // ADAM6X50_CONF_H
