#include "stdafx.h"
#include "VariableWriter.h"


VariableWriter::VariableWriter()
{
}


VariableWriter::~VariableWriter()
{
}

void VariableWriter::WriteCode(IndentationWriter & iw, Variable v)
{
	if (v.flags & mod_unsigned) {
		iw << "unsigned ";
	}
	if (v.flags & mod_struct) {
		iw << "struct ";
	}
	if (v.flags & mod_class) {
		iw << "class ";
	}
	iw << v.type;
	if (v.flags & mod_pointer) {
		iw << "*";
	}
	iw << " " << v.name;
	if (v.flags & mod_array) {
		iw << "[" << v.size << "]";
	}
	iw << ";" << endl;
}
