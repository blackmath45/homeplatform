#include "datatag.h"

void datatag::SetFromDBID(char * ID) { this->ID = atoi(ID); }
void datatag::SetFromDBMBAddress(char * MBAddress) { this->MBAddress = atoi(MBAddress); }
void datatag::SetFromDBMBType(char * MBType) { this->MBType = atoi(MBType); }
void datatag::SetFromDBValue(char * Value) { string str(Value); this->Value = str; }
void datatag::SetFromDBQuality(char * Quality) { this->Quality = atoi(Quality); }

void datatag::SetValue(string Value) { this->Value = Value; }
void datatag::SetQuality(int Quality) { this->Quality = Quality; }

int datatag::GetID() { return this->ID; }
int datatag::GetMBAddress() { return this->MBAddress; }
int datatag::GetMBType() { return this->MBType; }
string datatag::GetValue() { return this->Value; }
int datatag::GetQuality() { return this->Quality; }

void datatag::ToString() { cout << this->ID << " => " << this->MBAddress << " of " << this->MBType << "(" << this->Value << ")" << endl; }