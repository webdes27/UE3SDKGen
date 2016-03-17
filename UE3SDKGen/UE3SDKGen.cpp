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
#define DEBUG Log().Get(logDEBUG)
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

void destruct() {
	FreeLibraryAndExitThread(module, EXIT_SUCCESS);
	exit(-1);
}

Property_Type GetPropertyType(UProperty* prop) {
	static map<UClass*, Property_Type> types = 
	{
		{ UByteProperty::StaticClass(), Type_Byte },
		{ UIntProperty::StaticClass(), Type_Int },
		{ UFloatProperty::StaticClass(), Type_Float },
		{ UBoolProperty::StaticClass(), Type_Bool },
		{ UStrProperty::StaticClass(), Type_UString },
		{ UNameProperty::StaticClass(), Type_UName },
		{ UDelegateProperty::StaticClass(), Type_UDelegate },
		{ UObjectProperty::StaticClass(), Type_UObject },
		{ UClassProperty::StaticClass(), Type_UClass },
		{ UInterfaceProperty::StaticClass(), Type_UInterface },
		{ UStructProperty::StaticClass(), Type_UStruct },
		{ UArrayProperty::StaticClass(), Type_UArray },
		{ UMapProperty::StaticClass(), Type_UMap },
	};

	typedef std::map<UClass*, Property_Type>::iterator it_type;
	for (it_type iterator = types.begin(); iterator != types.end(); iterator++) {
		if (prop->IsA(iterator->first)) {
			return iterator->second;
		}
	}
	return Type_Unknown;
}

unsigned long GetPropertySize(Property_Type pType) {
	static map<Property_Type, unsigned long> sizes =
	{
		{ Type_Byte, sizeof(unsigned char) },
		{ Type_Int, sizeof(unsigned int) },
		{ Type_Float, sizeof(float) },
		{ Type_Bool, sizeof(bool) },
		{ Type_UString, sizeof(FString) },
		{ Type_UName, sizeof(FName) },
		{ Type_UDelegate, sizeof(FScriptDelegate) },
		{ Type_UObject, sizeof(void*) },
		{ Type_UClass, sizeof(void*) },
		{ Type_UInterface, sizeof(void*) },
		{ Type_UStruct, 0 },
		{ Type_UArray, sizeof(TArray<void*>) },
		{ Type_UMap, 0x3C }
	};
	return sizes[pType];
}

unsigned long GetPropertySize(UProperty* prop)
{
	Property_Type propType = GetPropertyType(prop);
	if (propType == Type_UStruct) {
		return prop->ElementSize;
	}
	return GetPropertySize(propType);
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
	DEBUG << "Trying to find address of current module";
	MODULEINFO currentGame = GetModuleInfo(NULL);
	DEBUG << "Game is at 0x" << currentGame.EntryPoint << ", size " << currentGame.SizeOfImage;

	DEBUG << "Trying to find GObjects location using pattern.";
	GObjects = *(unsigned long*)FindPattern(currentGame, (unsigned char*)GObjects_Pattern, (char*)GObjects_Mask, GObjects_Offset);
	if (GObjects == NULL) {
		ERRORLOG << "Could not find GObjects, check your pattern, mask & offset";
		destruct();
	}
	DEBUG << "GObjects found at 0x" << std::hex << GObjects << std::dec;

	DEBUG << "Trying to find GNames location using pattern.";
	GNames = *(unsigned long*)FindPattern(currentGame, (unsigned char*)GNames_Pattern, (char*)GNames_Mask, GNames_Offset);
	if (GNames == NULL) {
		ERRORLOG << "Could not find GNames, check your pattern, mask & offset";
		destruct();
	}
	DEBUG << "GNames found at 0x" << std::hex << GNames << std::dec;
}
vector<string> structsProcessed;
vector<Struct> structsFound;

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

void getStructs(UScriptStruct* ss, UObject* processPackage) {
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
		string superName = string(superStruct->GetFullName());
		if (find(structsProcessed.begin(), structsProcessed.end(), superName) == structsProcessed.end())
		{
			getStructs(superStruct, processPackage);
		}
	}

	for (UProperty* structProperty = (UProperty*)ss->Children; structProperty; structProperty = (UProperty*)structProperty->Next)
	{
		Property_Type type = GetPropertyType(structProperty);
		if (type == Type_UStruct) {
			UScriptStruct* scriptStruct = (UScriptStruct*)(((UStructProperty*)structProperty)->Struct);
			if (ss != scriptStruct && (find(structsProcessed.begin(), structsProcessed.end(), string(scriptStruct->GetFullName())) == structsProcessed.end())) {
				getStructs(scriptStruct, processPackage);
			}
		}

		if (type == Type_UArray) {
			UArrayProperty* arrayProp = (UArrayProperty*)structProperty;
			if (GetPropertyType(arrayProp->Inner) == Type_UStruct) {
				UScriptStruct* innerStruct = (UScriptStruct*)((UStructProperty*)arrayProp->Inner)->Struct;
				if (innerStruct == ss && find(structsProcessed.begin(), structsProcessed.end(), string(innerStruct->GetFullName())) == structsProcessed.end()) {
					getStructs(innerStruct, processPackage);
				}
			}
		}
	}
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
	DEBUG << "Starting on processing packages.";

	vector<UObject*> filteredPackages = filterGObjects([](UObject* obj) {
		return obj != NULL && obj->IsA(UClass::StaticClass()) && obj->GetPackageObj() != NULL;
	});

	vector<UObject*> checkedPackages;
	for (unsigned int i = 0; i <  filteredPackages.size(); i++) {
		UObject* package = filteredPackages.at(i)->GetPackageObj();

		if (!(find(checkedPackages.begin(), checkedPackages.end(), package) == checkedPackages.end()))
			continue;

		DEBUG << "Found package " << package->GetName() << ". Starting processing.";
		//extractStructs(package);
		DEBUG << "Done processing package " << package->GetName() << ". ";
		checkedPackages.push_back(package);
	}
	DEBUG << "Packages processed.";
}

void Run(HMODULE mod) {
	Log::ReportingLevel() = logDEBUG4;
	module = mod;
	initMemoryLocations();
	loadPackages();
}