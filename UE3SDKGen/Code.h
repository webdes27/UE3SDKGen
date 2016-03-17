#pragma once
#include <string>
#include <vector>
using namespace std;

enum Modifiers {
	mod_private = 0x01,
	mod_protected = 0x02,
	mod_public = 0x04,
	mod_unsigned = 0x08,
	mod_const = 0x10,
	mod_virtual = 0x20,
	mod_static = 0x40,
	mod_struct = 0x80,
	mod_class = 0x100,
	mod_array = 0x200,
	mod_pointer = 0x400,

};

struct Const {
	string key;
	string value;
};

struct EnumProperty {
	string key;
	int value;
};

struct Enum {
	string name;
	vector<EnumProperty> properties;
};

struct Function {
	string name;
	string type;
	long flags;
};

struct Variable {
	string name;
	string type;
	long flags;
	int size;
};

struct Struct {
	string name;
	string parentName;
	vector<Variable> variables;
};

struct Class {
	string name;
	string parentName;
	vector<Variable> variables;
	vector<Function> functions;
	vector<Struct> structs;
	vector<Enum> enums;
	vector<Const> constants;
};
