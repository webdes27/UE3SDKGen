#include "stdafx.h"
#include "UnrealClasses.h"


unsigned long GObjects = NULL;
unsigned long GNames = NULL;

TArray< UObject* >* UObject::GObjObjects()
{
	TArray< UObject* >* ObjectArray = (TArray< UObject* >*) GObjects;
	return ObjectArray;
}

char* UObject::GetName()
{
	static char nameBuffer[256];
	if (this->Name.Index < 0)
		return "NULLNAME";

	sprintf_s(nameBuffer, "%s", this->Name.GetName());

	return nameBuffer;
}

char* UObject::GetNameCPP()
{
	static char nameBuffer[256];

	if (this->IsA(UClass::StaticClass()))
	{
		UClass* pClass = (UClass*) this;
		while (pClass)
		{
			if (!strcmp(pClass->GetName(), "Actor"))
			{
				strcpy_s(nameBuffer, "A");
				break;
			}
			else if (!strcmp(pClass->GetName(), "Object"))
			{
				strcpy_s(nameBuffer, "U");
				break;
			}

			pClass = (UClass*)pClass->SuperField;
		}
	}
	else
	{
		strcpy_s(nameBuffer, "F");
	}

	strcat_s(nameBuffer, this->GetName());
	return nameBuffer;
}

char* UObject::GetFullName()
{
	if (this->Class && this->Outer)
	{
		static char cOutBuffer[512];

		char cTmpBuffer[512];

		strcpy_s(cOutBuffer, this->GetName());

		for (UObject* pOuter = this->Outer; pOuter; pOuter = pOuter->Outer)
		{
			strcpy_s(cTmpBuffer, pOuter->GetName());
			strcat_s(cTmpBuffer, ".");

			size_t len1 = strlen(cTmpBuffer);
			size_t len2 = strlen(cOutBuffer);

			memmove(cOutBuffer + len1, cOutBuffer, len1 + len2 + 1);
			memcpy(cOutBuffer, cTmpBuffer, len1);
		}

		strcpy_s(cTmpBuffer, this->Class->GetName());
		strcat_s(cTmpBuffer, " ");

		size_t len1 = strlen(cTmpBuffer);
		size_t len2 = strlen(cOutBuffer);

		memmove(cOutBuffer + len1, cOutBuffer, len1 + len2 + 1);
		memcpy(cOutBuffer, cTmpBuffer, len1);

		return cOutBuffer;
	}

	return "(null)";
}

char* UObject::GetPackageName()
{
	UObject* pPackage = this->GetPackageObj();

	if (pPackage)
	{
		static char cOutBuffer[256];
		strcpy_s(cOutBuffer, pPackage->GetName());
		return cOutBuffer;
	}
	else
	{
		return "(null)";
	}
}

UObject* UObject::GetPackageObj()
{
	UObject* pPackage = NULL;

	for (UObject* pOuter = this->Outer; pOuter; pOuter = pOuter->Outer)
	{
		pPackage = pOuter;
	}

	return pPackage;
}

template< class T > T* UObject::FindObject(char* ObjectFullName)
{
	while (!UObject::GObjObjects())
		Sleep(100);

	while (!FName::Names())
		Sleep(100);

	for (int i = 0; i < UObject::GObjObjects()->Count; ++i)
	{
		UObject* Object = UObject::GObjObjects()->Data[i];

		// skip no T class objects
		if
			(
				!Object
				|| !Object->IsA(T::StaticClass())
				)
			continue;

		// check
		if (!_stricmp(Object->GetFullName(), ObjectFullName))
			return (T*)Object;
	}

	return NULL;
}

template< class T > unsigned int UObject::CountObject(char* ObjectName)
{
	while (!UObject::GObjObjects())
		Sleep(100);

	while (!FName::Names())
		Sleep(100);

	static map< string, int > mCountCache;
	string sObjectName = string(ObjectName);

	if (mCountCache.count(sObjectName) == 0)
	{
		mCountCache[sObjectName] = 0;

		for (int i = 0; i < UObject::GObjObjects()->Count; ++i)
		{
			UObject* Object = UObject::GObjObjects()->Data[i];

			// skip no T class objects
			if
				(
					!Object
					|| !Object->IsA(T::StaticClass())
					)
				continue;

			// check
			if (!_stricmp(Object->GetName(), ObjectName))
			{
				mCountCache[sObjectName]++;
			}

		}
	}

	return mCountCache[sObjectName];
}

UClass* UObject::FindClass(char* ClassFullName)
{
	while (!UObject::GObjObjects())
		Sleep(100);

	while (!FName::Names())
		Sleep(100);

	for (int i = 0; i < UObject::GObjObjects()->Count; ++i)
	{
		UObject* Object = UObject::GObjObjects()->Data[i];

		if (!Object)
			continue;

		if (!_stricmp(Object->GetFullName(), ClassFullName))
			return (UClass*)Object;
	}

	return NULL;
}

signed int UObject::IsA(class UClass* SomeBase) const
{
	for (UClass* TempClass = Class; TempClass; TempClass = (UClass*)TempClass->SuperField)
		if (TempClass == SomeBase)
			return 1;
	return SomeBase == NULL;
}

signed int UObject::IsOfAny(std::initializer_list<UClass*> classes) const {
	for (UClass* c : classes) {
		if (IsA(c)) {
			return 1;
		}
	}
	return 0;
}

UClass*			UObject::pClassPointer = NULL;
UClass*			UField::pClassPointer = NULL;
UClass*			UEnum::pClassPointer = NULL;
UClass*			UConst::pClassPointer = NULL;
UClass*			UStruct::pClassPointer = NULL;
UClass*			UScriptStruct::pClassPointer = NULL;
UClass*			UFunction::pClassPointer = NULL;
UClass*			UState::pClassPointer = NULL;
UClass*			UClass::pClassPointer = NULL;
UClass*			UProperty::pClassPointer = NULL;
UClass*			UByteProperty::pClassPointer = NULL;
UClass*			UIntProperty::pClassPointer = NULL;
UClass*			UFloatProperty::pClassPointer = NULL;
UClass*			UBoolProperty::pClassPointer = NULL;
UClass*			UStrProperty::pClassPointer = NULL;
UClass*			UNameProperty::pClassPointer = NULL;
UClass*			UDelegateProperty::pClassPointer = NULL;
UClass*			UObjectProperty::pClassPointer = NULL;
UClass*			UClassProperty::pClassPointer = NULL;
UClass*			UInterfaceProperty::pClassPointer = NULL;
UClass*			UStructProperty::pClassPointer = NULL;
UClass*			UArrayProperty::pClassPointer = NULL;
UClass*			UMapProperty::pClassPointer = NULL;

