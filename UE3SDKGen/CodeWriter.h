#pragma once
#include <string>
#include <vector>
#include "IndentationWriter.h"
#include "Code.h"
using namespace std;
class StructWriter;

class CodeWriter
{
private:
	IndentationWriter iWriter;
protected:
	string name;
public:
	vector<Class> classes;
	vector<Struct> structs;
	vector<Const> consts;
	vector<Enum> enums;
	CodeWriter(string name);
	~CodeWriter();
	void AddClass(Class c);
	//virtual void WriteCode(IndentationWriter iw) = 0;
	void Save();
};
