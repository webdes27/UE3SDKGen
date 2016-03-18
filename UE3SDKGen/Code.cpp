#include "stdafx.h"
#include "Code.h"
Variable CreateVariable(string name, string type, long flags, int size) {
	Variable var;
	var.name = name;
	var.type = type;
	var.flags = flags;
	var.size = size;
	return var;
}