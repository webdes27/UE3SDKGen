// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <iostream>
#include <fstream>


using namespace std;

#include "UE3SDKGen.h"
//int main() {
//	CodeWriter* cw = new CodeWriter("test.cpp");
//	Class curClass;
//	curClass.name = "TestClass";
//	curClass.variables.push_back({ "test", "int", mod_unsigned | mod_private, 1});
//	curClass.variables.push_back({ "testi2", "int", mod_private, 1 });
//	curClass.variables.push_back({ "testi3", "int", mod_unsigned | mod_private | mod_array, 0x100});
//	curClass.variables.push_back({ "test2", "string", mod_public, 1});
//	curClass.variables.push_back({ "test3", "string", mod_public, 1 });
//	curClass.variables.push_back({ "test4", "string", mod_public, 1 });
//
//	curClass.constants.push_back({ "LOL", "123" });
//	Enum e;
//	e.properties.push_back({ "Yes", 0 });
//	e.properties.push_back({ "No", 1 });
//	e.properties.push_back({ "Maybe", 2 });
//	curClass.enums.push_back(e);
//	curClass.parentName = "object";
//
//	Struct s;
//	s.name = "TestStruct";
//	s.variables.push_back({ "test", "int", mod_unsigned | mod_private | mod_pointer, 1 });
//	s.variables.push_back({ "testi2", "int", mod_private, 1 });
//	s.variables.push_back({ "Distribution", "UDistributionFloat", mod_unsigned | mod_private | mod_array | mod_pointer | mod_class, 128 });
//	s.variables.push_back({ "test2", "string", mod_public, 1 });
//	s.variables.push_back({ "test3", "string", mod_public, 1 });
//	curClass.structs.push_back(s);
//	cw->AddClass(curClass);
//	
//	cw->Save();
//}

HMODULE module;

void OnAttach() {
	//Run(module);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: 
	{
		module = hModule;
		ofstream myfile;
		myfile.open("example.txt");
		myfile << "Writing this to a file.\n";
		myfile.close();

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OnAttach, NULL, 0, NULL);
		
	}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

