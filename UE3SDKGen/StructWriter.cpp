#include "stdafx.h"
#include "StructWriter.h"
#include <sstream>
#include "VariableWriter.h"

StructWriter::StructWriter()
{
	
}


StructWriter::~StructWriter()
{
}

void StructWriter::WriteCode(IndentationWriter& iw, Struct s)
{
	iw << "struct " << s.name;
	
	if (!s.parentName.empty()) {
		iw << " : " << s.parentName;
	}

	iw << endl << "{" << endl;
	iw.IncIndent();
	VariableWriter vw;
	for (unsigned int i = 0; i < s.variables.size(); i++) {
		Variable v = s.variables.at(i);
		vw.WriteCode(iw, v);
	}
	iw.DecIndent();
	iw << "};" << endl;
}
