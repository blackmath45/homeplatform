#include "adamtag.h"

void adamtag::SetQueryType(int Type) { this->QueryType = Type; };
void adamtag::SetMBAddress(int MBAddress) { this->MBAddress = MBAddress; };
void adamtag::SetValue(int Value) { this->Value = Value; }

int adamtag::GetQueryType() { return this->QueryType; }
int adamtag::GetMBAddress() { return this->MBAddress; }
int adamtag::GetValue() { return this->Value; }

void adamtag::ToString() { cout << this->QueryType << " => " << this->MBAddress << "(" << this->Value << ")" << endl; }
