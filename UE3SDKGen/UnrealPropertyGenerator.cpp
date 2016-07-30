#include "stdafx.h"
#include "UnrealPropertyGenerator.h"


string UnrealPropertyGenerator::getSafeName(string name)
{
	if (propertyMap.find(name) != propertyMap.end()) {
		propertyMap[name]++;
		return name + to_string(propertyMap[name]);
	}
	propertyMap.insert(make_pair(name, 0));
	return name;
}

UnrealPropertyGenerator::UnrealPropertyGenerator(DWORD size, DWORD curOffset) : totalSize(size), currentOffset(curOffset)
{
}


UnrealPropertyGenerator::~UnrealPropertyGenerator()
{
}

void UnrealPropertyGenerator::InsertProperty(UProperty * prop)
{
	
	if (currentOffset < prop->Offset) {
		DWORD offsetDiff = prop->Offset - currentOffset;
		if (offsetDiff >= CLASS_ALIGN) {
			Variable var = CreateVariable(
				string(getSafeName("UnknownData")),
				string("unsigned char"),
				(long)(mod_public | mod_array),
				(int)offsetDiff
				);
			var.comment = "padding";
			props.push_back(var);
		}
	}

	Property_Type propType = GetPropertyType(prop);
	if (propType != Type_Unknown)
	{
		Variable var;
		var.name = getSafeName(string(prop->GetName()));
		var.size = prop->ArrayDim;
		var.type = GetPropertyTypeName(prop);
		var.flags |= mod_public;
		if (var.size > 1) {
			var.flags |= mod_array;
		}
		props.push_back(var);

		int offsetError = (prop->ElementSize * prop->ArrayDim) - (GetPropertySize(prop) * prop->ArrayDim);
		if (offsetError > 0) {
			Variable var = CreateVariable(
				string(getSafeName("UnknownData")),
				string("unsigned char"),
				(long)(mod_public | mod_array),
				(int)offsetError
				);
			var.comment = "offset error padding";
			props.push_back(var);
		}
	}
	else 
	{
		Variable var = CreateVariable(
			string(getSafeName("UnknownData")),
			string("unsigned char"),
			(long)(mod_public | mod_array),
			(int)(prop->ElementSize * prop->ArrayDim)
			);
		var.comment = "unknown type";
		props.push_back(var);
	}
	currentOffset = prop->Offset + (prop->ElementSize * prop->ArrayDim);
	
}

void UnrealPropertyGenerator::Finish(int propertySize)
{
	if (propertySize > currentOffset) {
		DWORD offsetDiff = propertySize - currentOffset;
		if (offsetDiff >= CLASS_ALIGN) {
			Variable var = CreateVariable(
				string(getSafeName("UnknownData")),
				string("unsigned char"),
				(long)(mod_public | mod_array),
				(int)offsetDiff
				);
			props.push_back(var);
		}
	}
}

vector<Variable> UnrealPropertyGenerator::GetProperties()
{
	return props;
}
