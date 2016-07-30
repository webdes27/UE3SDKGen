#pragma once
#include "GameSpecific\unrealclasses.h"
#include <string>
#include <vector>

using namespace std;

enum Property_Type {
	Type_Byte,
	Type_Int,
	Type_Float,
	Type_Bool,
	Type_UString,
	Type_UName,
	Type_UDelegate,
	Type_UObject,
	Type_UClass,
	Type_UInterface,
	Type_UStruct,
	Type_UArray,
	Type_UMap,
	Type_Unknown
};

struct Property {
	UClass* static_class;
	Property_Type type;
	string type_name;
	unsigned long size;
};

bool SortProperty(UProperty* pPropertyA, UProperty* pPropertyB);
Property GetProperty(Property_Type prop);
Property GetProperty(UProperty* uProp);
Property_Type GetPropertyType(UProperty* uProp);
string GetPropertyTypeName(UProperty* uProp);
unsigned long GetPropertySize(UProperty* uProp);

template<typename T>
bool contains(T needle, vector<T> haystack) {
	return find(haystack.begin(), haystack.end(), needle) != haystack.end();
}