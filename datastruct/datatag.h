#ifndef DATATAG_H
#define DATATAG_H

#include <stdlib.h> 
#include <iostream>
#include <string>

using namespace std;

class datatag
{
private:
	int ID;
	int MBAddress;
	int MBType;
	string Value;
	int Quality;
	
public:
	void SetFromDBID(char * ID);
	void SetFromDBMBAddress(char * MBAddress);
	void SetFromDBMBType(char * MBType);
	void SetFromDBValue(char * Value);
	void SetFromDBQuality(char * Quality);

	void SetValue(string Value);
	void SetQuality(int Quality);
	
	int GetID();
	int GetMBAddress();
	int GetMBType();
	string GetValue();	
	int GetQuality();
	
	void ToString();
};

#endif // DATATAG_H






