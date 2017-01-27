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
	if (v.flags & mod_commented_out) {
		iw << "// ";
	}
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
	iw << "\t" << v.name;
	if (v.flags & mod_array) {
		iw << "[" << v.size << "]";
	}
	iw << ";";
	if (!v.comment.empty()) {
		iw << " //" << v.comment;
	}
	iw << endl;
}
