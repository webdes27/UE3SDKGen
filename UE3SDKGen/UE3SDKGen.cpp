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
#include "UnrealPropertyGenerator.h"
#include "UnrealProperty.h"

#define DEBUGLOG Log().Get(logDEBUG)
#define INFOLOG Log().Get(logINFO)
#define WARNLOG Log().Get(logWARN)
#define ERRORLOG Log().Get(logERROR)

extern vector<Property> properties;
extern unsigned long GObjects;
extern unsigned long GNames;

HMODULE gameModule;

void destruct() {
	FreeLibraryAndExitThread(gameModule, EXIT_SUCCESS);
	exit(-1);
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



UScriptStruct* findBiggestScriptStruct(UScriptStruct* ss) {
	unsigned long biggestSize = ss->PropertySize;
	UScriptStruct* biggestStruct = ss;
	string scriptStructName = string(ss->GetFullName());
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
		if (pProperty->ElementSize > 0 && !pProperty->IsA(UScriptStruct::StaticClass()))
			propertyList.push_back(pProperty);
		else if (pProperty->IsA(UFunction::StaticClass()))
			continue;
	}
	sort(propertyList.begin(), propertyList.end(), SortProperty);

	UnrealPropertyGenerator upg(0, 0);

	for (unsigned int i = 0; i < propertyList.size(); i++) {
		UProperty* prop = propertyList.at(i);
		upg.InsertProperty(prop);
	}
	upg.Finish(scriptStruct->PropertySize);
	vector<Variable> vars = upg.GetProperties();
	s.variables.insert(s.variables.end(), vars.begin(), vars.end());
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

	ss = findBiggestScriptStruct(ss);

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
	std::vector<UObject*> filteredObjects = filterGObjects([&](UObject* obj) {
		return obj != NULL && obj->GetPackageObj() != NULL && obj->IsA(UScriptStruct::StaticClass()) && processPackage == obj->GetPackageObj();
	});

	for (unsigned int i = 0; i < filteredObjects.size(); i++) {
		UObject* obj = filteredObjects.at(i);
		getStructs((UScriptStruct*)obj, processPackage);
	}
	return structs;
}

vector<Class> classesFound;
void getClass(UClass* c) {
	vector< UProperty* > propertyList;
	for (UProperty* pProperty = (UProperty*)c->Children; pProperty; pProperty = (UProperty*)pProperty->Next)
	{
		if (pProperty->ElementSize > 0 && !pProperty->IsOfAny({ UFunction::StaticClass(), UConst::StaticClass(), UEnum::StaticClass(), UScriptStruct::StaticClass() }))
			propertyList.push_back(pProperty);
	}
	sort(propertyList.begin(), propertyList.end(), SortProperty);

	
	UClass* superClass = (UClass*)c->SuperField;
	UnrealPropertyGenerator upg(0, 0);
	Class codeClass;
	codeClass.name = string(c->GetNameCPP());

	if (superClass != NULL && superClass != c) {
		upg.totalSize = c->PropertySize - superClass->PropertySize;
		upg.currentOffset = superClass->PropertySize;
		codeClass.parentName = string(superClass->GetNameCPP());
	}


	for (unsigned int i = 0; i < propertyList.size(); i++) {
		UProperty* prop = propertyList.at(i);
		upg.InsertProperty(prop);
	}
	upg.Finish(c->PropertySize);
	vector<Variable> vars = upg.GetProperties();
	codeClass.variables.insert(codeClass.variables.end(), vars.begin(), vars.end());
	classesFound.push_back(codeClass);
	
}

void getClasses(UClass* c, UObject* processPackage) {
	UObject* package = c->GetPackageObj();
	if (package == NULL)
		return;

	static vector<string> generatedClasses;
	string className = string(c->GetFullName());
	if (!contains(className, generatedClasses)) {
		if (c->SuperField != NULL && c->SuperField != c && contains(string(c->SuperField->GetFullName()), generatedClasses)) {
			getClasses((UClass*)c->SuperField, processPackage);
		}
		getClass(c);
		generatedClasses.push_back(className);
	}
	
}


void extractClasses(UObject* processPackage) {
	std::vector<UObject*> filteredObjects = filterGObjects([&](UObject* obj) {
		return obj != NULL && obj->GetPackageObj() != NULL && obj->IsA(UClass::StaticClass()) && obj->GetPackageObj() == processPackage;
	});
	for (unsigned int i = 0; i < filteredObjects.size(); i++) {
		UObject* obj = filteredObjects.at(i);
		getClasses((UClass*)obj, processPackage);
	}
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
		extractStructs(package);
		extractClasses(package);
		CodeWriter cw("Generated/" + string(package->GetName()));
		cw.classes.insert(cw.classes.end(), classesFound.begin(), classesFound.end());
		cw.structs.insert(cw.structs.end(), structsFound.begin(), structsFound.end());
		cw.Save();

		DEBUGLOG << "Done processing package " << package->GetName() << ". ";
		structsFound.clear();
		classesFound.clear();
		checkedPackages.push_back(package);
	}
	DEBUGLOG << "Packages processed.";
}

void initProperties() {
	properties =
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
}

void Run(HMODULE mod) {
	Log::ReportingLevel() = logDEBUG4;
	gameModule = mod;
	initMemoryLocations();
	initProperties(); //Has to be done here. after GNames and GObjects are found...
	loadPackages();
}