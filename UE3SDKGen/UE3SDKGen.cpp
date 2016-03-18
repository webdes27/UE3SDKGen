// UE3SDKGen.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CodeWriter.h"
#include "HackHelpers.h"
#include "Logger.h"
#include "GameSpecific/RocketLeague.h"
#include <functional>
#include <algorithm>
#include <map>

#define DEBUGLOG Log().Get(logDEBUG)
#define INFOLOG Log().Get(logINFO)
#define WARNLOG Log().Get(logWARN)
#define ERRORLOG Log().Get(logERROR)
extern HMODULE module;



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

static vector<Property> properties = 
{
	{ UByteProperty::StaticClass(), Type_Byte, "unsigned char", sizeof(unsigned char) },
	{ UIntProperty::StaticClass(), Type_Int, "int", sizeof(unsigned int) },
	{ UFloatProperty::StaticClass(), Type_Float, "float", sizeof(float) },
	{ UBoolProperty::StaticClass(), Type_Bool, "bool", sizeof(bool) },
	{ UStrProperty::StaticClass(), Type_UString, "struct FString", sizeof(FString) },
	{ UNameProperty::StaticClass(), Type_UName, "struct FName", sizeof(FName) },
	{ UDelegateProperty::StaticClass(), Type_UDelegate, "struct FScriptDelegate", sizeof(FScriptDelegate) },
	{ UObjectProperty::StaticClass(), Type_UObject, "", sizeof(void*) },
	{ UClassProperty::StaticClass(), Type_UClass, "", sizeof(void*) },
	{ UInterfaceProperty::StaticClass(), Type_UInterface, "", sizeof(void*) },
	{ UStructProperty::StaticClass(), Type_UStruct, "", 0 },
	{ UArrayProperty::StaticClass(), Type_UArray, "", sizeof(TArray<void*>) },
	{ UMapProperty::StaticClass(), Type_UMap, "", 0x3C },
	{ NULL, Type_Unknown, "ERROR", 0x0 },
};



void destruct() {
	FreeLibraryAndExitThread(module, EXIT_SUCCESS);
	exit(-1);
}

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

Property GetProperty(UProperty* prop) {
	for (unsigned int i = 0; i < properties.size(); i++) {
		Property tempProp = properties.at(i);
		if (prop->IsA(tempProp.static_class)) {
			return tempProp;
		}
	}
	return GetProperty(Type_Unknown);
}

Property_Type GetPropertyType(UProperty* uProp) {
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

typedef function<bool(UObject*)> filter;

std::vector<UObject*> filterGObjects(filter f) {
	std::vector<UObject*> matching;
	TArray<UObject*>* objects = UObject::GObjObjects();

	for (unsigned int i = 0; i < objects->Num(); i++) {
		if (f(objects->Data[i])) {
			matching.push_back(objects->Data[i]);
		}
	}
	return matching;
}

void initMemoryLocations() {
	DEBUGLOG << "Trying to find address of current module";
	MODULEINFO currentGame = GetModuleInfo(NULL);
	DEBUGLOG << "Game is at 0x" << currentGame.EntryPoint << ", size " << currentGame.SizeOfImage;

	DEBUGLOG << "Trying to find GObjects location using pattern.";
	GObjects = *(unsigned long*)FindPattern(currentGame, (unsigned char*)GObjects_Pattern, (char*)GObjects_Mask, GObjects_Offset);
	if (GObjects == NULL) {
		ERRORLOG << "Could not find GObjects, check your pattern, mask & offset";
		destruct();
	}
	DEBUGLOG << "GObjects found at 0x" << std::hex << GObjects << std::dec;

	DEBUGLOG << "Trying to find GNames location using pattern.";
	GNames = *(unsigned long*)FindPattern(currentGame, (unsigned char*)GNames_Pattern, (char*)GNames_Mask, GNames_Offset);
	if (GNames == NULL) {
		ERRORLOG << "Could not find GNames, check your pattern, mask & offset";
		destruct();
	}
	DEBUGLOG << "GNames found at 0x" << std::hex << GNames << std::dec;
}



UScriptStruct* findBiggestScriptStruct(string scriptStructName) {
	unsigned long biggestSize = -1;
	UScriptStruct* biggestStruct = NULL;
	std::vector<UObject*> filteredScriptStruct = filterGObjects([&](UObject* obj) {
		return obj != NULL && obj->IsA(UScriptStruct::StaticClass()) && string(obj->GetFullName()) == scriptStructName;
	});

	for (unsigned int i = 0; i < filteredScriptStruct.size(); i++) {
		UScriptStruct* currentStruct = (UScriptStruct*)filteredScriptStruct.at(i);
		if (currentStruct->PropertySize >= biggestSize) {
			biggestSize = currentStruct->PropertySize;
			biggestStruct = currentStruct;
		}
	}
	return biggestStruct;
}

vector<Struct> structsFound;
void saveStruct(UScriptStruct* scriptStruct) {
	Struct s;
	s.name = string(scriptStruct->GetNameCPP());

	if (scriptStruct->SuperField != NULL && scriptStruct->SuperField != scriptStruct) {
		s.parentName = string(scriptStruct->SuperField->GetNameCPP());
	}

	vector< UProperty* > propertyList;
	for (UProperty* pProperty = (UProperty*)scriptStruct->Children; pProperty; pProperty = (UProperty*)pProperty->Next)
	{
		if(pProperty->ElementSize > 0 && !pProperty->IsA(UScriptStruct::StaticClass()))
			propertyList.push_back(pProperty);
	}
	sort(propertyList.begin(), propertyList.end(), SortProperty);

	for (unsigned int i = 0; i < propertyList.size(); i++) {
		UProperty* prop = propertyList.at(i);
		Property_Type propType = GetPropertyType(prop);
		Variable var;
		var.name = string(prop->GetName());
		var.size = prop->ArrayDim;
		var.type = GetPropertyTypeName(prop);
		var.flags |= mod_public;
		s.variables.push_back(var);
	}
	structsFound.push_back(s);
}


void getStructs(UScriptStruct* ss, UObject* processPackage) {
	static vector<string> structsProcessed;
	UObject* package = ss->GetPackageObj();
	if (package == NULL || package != processPackage)
		return;

	string scriptStructName = string(ss->GetFullName());
	if (find(structsProcessed.begin(), structsProcessed.end(), scriptStructName) != structsProcessed.end())
		return;

	ss = findBiggestScriptStruct(scriptStructName);

	if (ss->SuperField != NULL && ss->SuperField != ss && ss->SuperField->IsA(UScriptStruct::StaticClass()))
	{
		UScriptStruct* superStruct = (UScriptStruct*)ss->SuperField;
		getStructs(superStruct, processPackage);
	}

	for (UProperty* structProperty = (UProperty*)ss->Children; structProperty; structProperty = (UProperty*)structProperty->Next)
	{
		Property_Type type = GetPropertyType(structProperty);
		if (type == Type_UStruct) {
			UScriptStruct* scriptStruct = (UScriptStruct*)(((UStructProperty*)structProperty)->Struct);
			if (ss != scriptStruct) {
				getStructs(scriptStruct, processPackage);
			}
		}

		if (type == Type_UArray) {
			UArrayProperty* arrayProp = (UArrayProperty*)structProperty;
			if (GetPropertyType(arrayProp->Inner) == Type_UStruct) {
				UScriptStruct* innerStruct = (UScriptStruct*)((UStructProperty*)arrayProp->Inner)->Struct;
				if (innerStruct != ss) {
					getStructs(innerStruct, processPackage);
				}
			}
		}
	}
	saveStruct(ss);
	structsProcessed.push_back(scriptStructName);
}

vector<Struct> extractStructs(UObject* processPackage) {
	vector<Struct> structs;
	std::vector<UObject*> filteredObjects = filterGObjects([](UObject* obj) {
		return obj != NULL && obj->GetPackageObj() != NULL && obj->IsA(UScriptStruct::StaticClass());
	});

	for (unsigned int i = 0; i < filteredObjects.size(); i++) {
		UObject* obj = filteredObjects.at(i);
		if (processPackage == obj->GetPackageObj()) {
			getStructs((UScriptStruct*)obj, processPackage);
		}
	}
	return structs;
}


void loadPackages() {
	DEBUGLOG << "Starting on processing packages.";

	vector<UObject*> filteredPackages = filterGObjects([](UObject* obj) {
		return obj != NULL && obj->IsA(UClass::StaticClass()) && obj->GetPackageObj() != NULL;
	});

	vector<UObject*> checkedPackages;
	for (unsigned int i = 0; i <  filteredPackages.size(); i++) {
		UObject* package = filteredPackages.at(i)->GetPackageObj();

		if (!(find(checkedPackages.begin(), checkedPackages.end(), package) == checkedPackages.end()))
			continue;

		DEBUGLOG << "Found package " << package->GetName() << ". Starting processing.";
		//extractStructs(package);
		//CodeWriter cw(package->GetName());
		//Class c;
		//c.structs.insert(c.structs.end(), structsFound.begin(), structsFound.end());
		//cw.AddClass(c);
		//cw.Save();

		DEBUGLOG << "Done processing package " << package->GetName() << ". ";
		checkedPackages.push_back(package);
	}
	DEBUGLOG << "Packages processed.";
}

void Run(HMODULE mod) {
	Log::ReportingLevel() = logDEBUG4;
	module = mod;
	initMemoryLocations();
	loadPackages();
}