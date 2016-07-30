#include "stdafx.h"
#include "UnrealProperty.h"

vector<Property> properties;
bool SortProperty(UProperty* pPropertyA, UProperty* pPropertyB)
{
	if
		(
			pPropertyA->Offset == pPropertyB->Offset
			&&	pPropertyA->IsA(UBoolProperty::StaticClass())
			&& pPropertyB->IsA(UBoolProperty::StaticClass())
			)
		return (((UBoolProperty*)pPropertyA)->BitMask < ((UBoolProperty*)pPropertyB)->BitMask);
	else
		return (pPropertyA->Offset < pPropertyB->Offset);
}

Property GetProperty(Property_Type prop) {
	for (unsigned int i = 0; i < properties.size(); i++) {
		Property tempProp = properties.at(i);
		if (tempProp.type == prop) {
			return tempProp;
		}
	}
	return GetProperty(Type_Unknown);
}

Property GetProperty(UProperty* uProp) {
	if (uProp == NULL) {
		return GetProperty(Type_Unknown);
	}
	for (unsigned int i = 0; i < properties.size(); i++) {
		Property tempProp = properties.at(i);
		if (uProp->IsA(tempProp.static_class)) {
			return tempProp;
		}
	}
	return GetProperty(Type_Unknown);
}

Property_Type GetPropertyType(UProperty* uProp) {
	if (uProp == NULL) {
		return Type_Unknown;
	}
	for (unsigned int i = 0; i < properties.size(); i++) {
		Property prop = properties.at(i);
		if (uProp->IsA(prop.static_class)) {
			return prop.type;
		}
	}
	return Type_Unknown;
}

string GetPropertyTypeName(UProperty* uProp) {
	Property_Type type = GetPropertyType(uProp);
	switch (type) {
	case Type_UObject:
		return "class " + (string(((UObjectProperty*)uProp)->PropertyClass->GetNameCPP())) + "*";
		break;
	case Type_UClass:
		return "class " + string(((UClassProperty*)uProp)->MetaClass->GetNameCPP()) + "*";
		break;
	case Type_UInterface:
		return "class " + string(((UInterfaceProperty*)uProp)->InterfaceClass->GetNameCPP()) + "*";
		break;
	case Type_UStruct:
		return "struct " + string(((UStructProperty*)uProp)->Struct->GetNameCPP());
		break;
	case Type_UArray:
	{
		UProperty* innerProp = ((UArrayProperty*)uProp)->Inner;
		Property_Type innerPropType = GetPropertyType(innerProp);
		if (innerPropType != Type_Unknown) {
			string innerPropName = GetPropertyTypeName(innerProp);
			return "TArray<" + innerPropName + ">";
		}
		else {
			return "ERROR_TARRAY";
		}
		break;
	}
	case Type_UMap:
	{
		UMapProperty* mapProp = (UMapProperty*)uProp;
		string keyName = GetPropertyTypeName(mapProp->Key);
		string valueName = GetPropertyTypeName(mapProp->Value);
		return "TMap<" + keyName + ", " + valueName + ">";
		break;
	}
	default:
		return GetProperty(uProp).type_name;
		break;
	}
}

unsigned long GetPropertySize(UProperty* uProp) {
	Property_Type type = GetPropertyType(uProp);
	if (type == Type_UStruct) {
		return uProp->ElementSize;
	}
	return GetProperty(type).size;
}