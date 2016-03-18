#include "..\stdafx.h"
#pragma once

#ifdef _MSC_VER
#pragma pack ( push, 0x4 )
#endif

extern unsigned long GObjects;
extern unsigned long GNames;

//typedefs
typedef int NAME_INDEX;
typedef void* FPointer;
typedef unsigned __int64	QWORD;
class UObject;



/*
# ========================================================================================= #
# Structs
# ========================================================================================= #
*/

template< class T > struct TArray
{
public:
	T* Data;
	int Count;
	int Max;

public:
	TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};

	T& operator() (int i)
	{
		return this->Data[i];
	};

	const T& operator() (int i) const
	{
		return this->Data[i];
	};

	int Num()
	{
		return this->Count;
	};

	void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	void Clear()
	{
		free(Data);
		Count = Max = 0;
	};
};

struct FNameEntry
{
	NAME_INDEX		Index;
	QWORD			Flags;
	FNameEntry*		HashNext;
	char			Name[0x10];
};

struct FName
{
	NAME_INDEX		Index;
	unsigned char	unknownData00[0x4];

	FName() : Index(0) {};

	FName(int i) : Index(i) {};

	~FName() {};

	FName(char* FindName)
	{
		static TArray< int > NameCache;

		for (int i = 0; i < NameCache.Count; ++i)
		{
			if (!strcmp(this->Names()->Data[NameCache(i)]->Name, FindName))
			{
				Index = NameCache(i);
				return;
			}
		}

		for (int i = 0; i < this->Names()->Count; ++i)
		{
			if (this->Names()->Data[i])
			{
				if (!strcmp(this->Names()->Data[i]->Name, FindName))
				{
					NameCache.Add(i);
					Index = i;
				}
			}
		}
	};

	static TArray< FNameEntry* >* Names()
	{
		return (TArray< FNameEntry* >*) GNames;
	};

	char* GetName()
	{
		if (Index < 0 || Index > this->Names()->Num())
			return "UnknownName";
		else
			return this->Names()->Data[Index]->Name;
	};

	bool operator == (const FName& A) const
	{
		return (Index == A.Index);
	};
};

struct FString : public TArray< wchar_t >
{
	FString() {};

	FString(wchar_t* Other)
	{
		this->Max = this->Count = *Other ? (wcslen(Other) + 1) : 0;

		if (this->Count)
			this->Data = Other;
	};

	~FString() {};

	FString operator = (wchar_t* Other)
	{
		if (this->Data != Other)
		{
			this->Max = this->Count = *Other ? (wcslen(Other) + 1) : 0;

			if (this->Count)
				this->Data = Other;
		}

		return *this;
	};
};

struct FScriptDelegate
{
	UObject* Object;
	FName FunctionName;
};




class UObject
{
public:
	FPointer				VfTableObject;							// 0x0000 (0x04)
	char					unknown_data00[0x1c];					// 0x0004 (0x1c)
	int						ObjectInternalInteger;					// 0x0020 (0x04)
	char					unknown_data01[0x4];
	class UObject*			Outer;                                  // 0x0028 (0x04)
	struct FName			Name;                                   // 0x002C (0x08)
	class UClass*			Class;                                  // 0x0034 (0x04)
	class UObject*			ObjectArchetype;						// 0x0038 (0x04)

private:
	static UClass* pClassPointer;

public:
	static TArray< UObject* >* GObjObjects();

	char* GetName();
	char* GetNameCPP();
	char* GetFullName();
	char* GetPackageName();
	UObject* GetPackageObj();

	template< class T > static T* FindObject(char* ObjectFullName);
	template< class T > static unsigned int CountObject(char* ObjectName);
	static UClass* FindClass(char* ClassFullName);

	signed int UObject::IsA(class UClass* SomeBase) const;

	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Object");

		return pClassPointer;
	};
};

// (0x003C - 0x0040)
class UField : public UObject
{
public:
	class UField*		Next;										// 0x003C (0x04)


private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Field");

		return pClassPointer;
	};
};

// (0x0040 - 0x004C)
class UEnum : public UField
{
public:
	TArray< FName >	Names;											// 0x0040 (0x0C)	

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Enum");

		return pClassPointer;
	};
};

// (0x0040 - 0x004C)
class UConst : public UField
{
public:
	struct FString		Value;										// 0x0040 (0x0C)													

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Const");

		return pClassPointer;
	};
};

// (0x0040 - 0x0090)
class UStruct : public UField
{
public:
	unsigned char			UnknownData00[0x8];					// 0x0040 (0x08)
	class UField*			SuperField;								// 0x0048 (0x04)
	class UField*			Children;								// 0x004C (0x04)
	unsigned long			PropertySize;							// 0x0050 (0x04)
	unsigned char			UnknownData01[0x30];					// 0x0054 (0x30)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Struct");

		return pClassPointer;
	};
};

// (0x0090 - 0x00AC)
class UScriptStruct : public UStruct
{
public:
	unsigned char		UnknownData00[0x1C];						// 0x0090 (0x1C)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.ScriptStruct");

		return pClassPointer;
	};
};

// (0x0090 - 0x00B0)
class UFunction : public UStruct
{
public:
	unsigned long		FunctionFlags;								// 0x0090 (0x04)
	unsigned short		iNative;									// 0x0094 (0x02)
	unsigned short		RepOffset;									// 0x0096 (0x02)
	struct FName		FriendlyName;								// 0x0098 (0x08)
	unsigned short		NumParms;									// 0x00A0 (0x02)
	unsigned short		ParmsSize;									// 0x00A2 (0x02)
	unsigned long		ReturnValueOffset;							// 0x00A4 (0x04)
	unsigned char		UnknownData00[0x4];						// 0x00A8 (0x04)
	void*				Func;										// 0x00AC (0x04) //unknowndata might be func?

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Function");

		return pClassPointer;
	};
};

// (0x0090 - 0x00D8)
class UState : public UStruct
{
public:
	//QWORD ProbeMask;
	//QWORD IgnoreMask;
	//DWORD StateFlags;
	//unsigned short LabelTableOffset;
	//TMap<FName, UFunction*> FuncMap;
	unsigned char			UnknownData00[0x48];						// 0x0090 (0x48)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.State");

		return pClassPointer;
	};
};

// (0x00D8 - 0x01D4)
class UClass : public UState
{
public:
	/*DWORD				ClassFlags;
	INT					ClassUnique;
	FGuid				ClassGuid;
	UClass*				ClassWithin;
	FName				ClassConfigName;
	TArray<FRepRecord>	ClassReps;
	TArray<UField*>		NetFields;
	TArray<FName>		PackageImports;
	TArray<FName>		HideCategories;
	TArray<FName>		AutoExpandCategories;
	TArray<FName>       DependentOn;
	FString				ClassHeaderFilename;*/
	unsigned char			UnknownData00[0x178];					// 0x00D8 (0xFC)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Class");

		return pClassPointer;
	};
};

// (0x0040 - 0x0080)
class UProperty : public UField
{
public:
	unsigned long		ArrayDim;									// 0x0040 (0x04)					
	unsigned long		ElementSize;								// 0x0044 (0x04)
	QWORD				PropertyFlags;								// 0x0048 (0x08)
	unsigned long		PropertySize;

	unsigned char		UnknownData00[0xC];						// 0x0050 (0x10) //fname category?
	unsigned long		Offset;										// 0x0060 (0x04)
	unsigned char		UnknownData01[0xC];						// 0x0064 (0x1C)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.Property");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UByteProperty : public UProperty
{
public:
	class UEnum*		Enum;										// 0x0088 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.ByteProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0080)
class UIntProperty : public UProperty
{
public:

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.IntProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0080)
class UFloatProperty : public UProperty
{
public:

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.FloatProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UBoolProperty : public UProperty
{
public:
	unsigned long		BitMask;									// 0x0080 (0x04)			

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.BoolProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0080)
class UStrProperty : public UProperty
{
public:

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.StrProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0080)
class UNameProperty : public UProperty
{
public:

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.NameProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0088)
class UDelegateProperty : public UProperty
{
public:
	UFunction*	Function;
	FName		DelegateName; //total 12 bytes though
							  //unsigned char			UnknownData00[0x8];						// 0x0080 (0x08)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.DelegateProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UObjectProperty : public UProperty
{
public:
	class UClass*		PropertyClass;								// 0x0080 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.ObjectProperty");

		return pClassPointer;
	};
};

// (0x0084 - 0x0088)
class UClassProperty : public UObjectProperty
{
public:
	class UClass*		MetaClass;									// 0x0084 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.ClassProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UInterfaceProperty : public UProperty
{
public:
	class UClass*		InterfaceClass;								// 0x0080 (0x04) 

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.InterfaceProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UStructProperty : public UProperty
{
public:
	class UStruct*		Struct;										// 0x0080 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.StructProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0084)
class UArrayProperty : public UProperty
{
public:
	class UProperty*	Inner;										// 0x0080 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.ArrayProperty");

		return pClassPointer;
	};
};

// (0x0080 - 0x0088)
class UMapProperty : public UProperty
{
public:
	class UProperty*	Key;										// 0x0080 (0x04)
	class UProperty*	Value;										// 0x0084 (0x04)

private:
	static UClass* pClassPointer;

public:
	static UClass* StaticClass()
	{
		if (!pClassPointer)
			pClassPointer = UObject::FindClass("Class Core.MapProperty");

		return pClassPointer;
	};
};



#ifdef _MSC_VER
#pragma pack ( pop )
#endif