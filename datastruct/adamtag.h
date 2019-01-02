#ifndef ADAMTAG_H
#define ADAMTAG_H

#include <stdlib.h> 
#include <iostream>
#include <string>

using namespace std;

class adamtag
{
private:
	int QueryType; // 1=ReadToPLC, 2=WriteToPLC, -1=ReadResponse, -2=WriteResponse
	int MBAddress;
	int Value;
	
public:
	void SetQueryType(int Type);
	void SetMBAddress(int MBAddress);
	void SetValue(int Value);
	
	int GetQueryType();
	int GetMBAddress();
	int GetValue();	
	
	void ToString();	
};

#endif // ADAMTAG_H
