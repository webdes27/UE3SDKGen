#pragma once
#include <map>
#include <string>
#include "GameSpecific\RocketLeague.h"
#include "GameSpecific\unrealclasses.h"
#include "UnrealProperty.h"
#include "Code.h"
using namespace std;

class UnrealPropertyGenerator
{
private:
	string getSafeName(string name);
	vector<Variable> props;
public:
	map<string, int> propertyMap = { { "UnknownData", 0 } };
	DWORD totalSize = 0;
	DWORD currentOffset = 0;
	UnrealPropertyGenerator(DWORD size, DWORD curOffset);
	~UnrealPropertyGenerator();
	void InsertProperty(UProperty* prop);
	void Finish(int propertySize);
	vector<Variable> GetProperties();
};

